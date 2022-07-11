#include "TextDrawer.h"
#include "RDirectX.h"
#include "Util.h"

using namespace std;

Texture TextDrawer::GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize, bool useAlign)
{
	wstring wGlyph = Util::ConvertStringToWString(glyph);
	wstring wFontTypeFace = Util::ConvertStringToWString(fontTypeFace);
	return GetFontTexture(wGlyph, wFontTypeFace, fontSize, useAlign);
}

Texture TextDrawer::GetFontTexture(std::wstring glyph, std::wstring fontTypeFace, UINT fontSize, bool useAlign)
{
	TextDrawer* drawer = GetInstance();

	if (glyph.length() != 1) {
		return Texture();
	}

	Glyph glyphData = { glyph, fontTypeFace, fontSize };

	auto itr = drawer->glyphMap.find(glyphData);
	if (itr != drawer->glyphMap.end()) {
		return itr->second;
	}

	TEXTMETRIC tm;
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };

	HFONT _font = CreateFont(fontSize, 0, 0, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, fontTypeFace.c_str());

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, _font);
	UINT code = (UINT)*glyph.c_str();

	OutputDebugString((glyph + L"\n").c_str());

	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetTextMetrics(hdc, &tm);
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// オブジェクトの開放
	SelectObject(hdc, oldFont);
	DeleteObject(_font);
	ReleaseDC(NULL, hdc);

	int fontWidth = (GM.gmBlackBoxX + 3) / 4 * 4;
	int fontHeight = GM.gmBlackBoxY;

	int baseLineY = tm.tmAscent;
	int glyphOriginY = 0;

	if (useAlign) {
		fontHeight = tm.tmHeight;
		glyphOriginY = baseLineY - GM.gmptGlyphOrigin.y;
	}

	int fontDataCount = fontWidth * fontHeight;

	OutputDebugString((wstring(L"fontWidth: ") + to_wstring(fontWidth) + L"(" + to_wstring(GM.gmBlackBoxX) + L")\n").c_str());
	OutputDebugString((wstring(L"fontHeight: ") + to_wstring(fontHeight) + L"\n").c_str());

	Color* imageData = new Color[fontDataCount];
	for (size_t i = 0; i < fontDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	for (size_t i = 0; i < size; i++) {
		size_t posX = i % fontWidth;
		size_t posY = glyphOriginY + (i / fontWidth);

		size_t access = (posY * fontWidth) + posX;

		float grayScale = (float)ptr[i];
		imageData[access].r = 1.0f;
		imageData[access].g = 1.0f;
		imageData[access].b = 1.0f;
		imageData[access].a = (grayScale * 255.0f / 16) / 255.0f;
	}

	Texture texture = Texture();

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = fontWidth;
	textureResourceDesc.Height = fontHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	RDirectX::GetInstance()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);

	texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * fontWidth,
		sizeof(Color) * fontDataCount
	);

	delete[] imageData;

	if (drawer->nextIndex >= TextDrawer::numSRVDescritors) {
		drawer->nextIndex = 0;
	}

	std::list<Glyph> eraseList;
	for (std::pair<Glyph, Texture> p : drawer->glyphMap) {
		if (p.second.heapIndex == drawer->nextIndex) {
			eraseList.push_back(p.first);
		}
	}

	for (Glyph g : eraseList) {
		drawer->glyphMap.erase(g);
	}

	//シェーダーリソースビュー
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = drawer->srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = drawer->srvHeap->GetGPUDescriptorHandleForHeapStart();
	size_t incrementSize = RDirectX::GetInstance()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += drawer->nextIndex * incrementSize;
	_gpuHandle.ptr += drawer->nextIndex * incrementSize;
	texture.cpuHandle = _cpuHandle;
	texture.gpuHandle = _gpuHandle;
	texture.heapIndex = drawer->nextIndex;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.resource->GetDesc().Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture.resource->GetDesc().MipLevels;

	//生成
	RDirectX::GetInstance()->device->CreateShaderResourceView(texture.resource.Get(), &srvDesc, _cpuHandle);

	drawer->glyphMap[glyphData] = texture;

	return texture;
}

TextureHandle TextDrawer::CreateStringTexture(std::string text, std::string fontTypeFace, UINT fontSize, std::string handle)
{
	wstring wText = Util::ConvertStringToWString(text);
	wstring _wTypeFace = Util::ConvertStringToWString(fontTypeFace);

	list<Texture> glyphlist;

	for (size_t i = 0; i < wText.size(); i++) {
		glyphlist.push_back(GetFontTexture(wText.substr(i, 1), _wTypeFace, fontSize, true));
	}

	UINT64 textureWidth = 0;
	UINT textureHeight = 0;

	for (Texture tex : glyphlist) {
		textureWidth += tex.resource->GetDesc().Width;

		UINT height = tex.resource->GetDesc().Height;
		if (textureHeight < height) {
			textureHeight = height;
		}
	}

	UINT64 imageDataCount = textureWidth * textureHeight;

	Color* imageData = new Color[imageDataCount];
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	UINT64 currentPos = 0;

	for (Texture tex : glyphlist) {
		size_t _width = tex.resource->GetDesc().Width;
		size_t _height = tex.resource->GetDesc().Height;
		size_t _dataCount = _width * _height;

		Color* _image = new Color[_dataCount];

		HRESULT result = tex.resource->ReadFromSubresource(_image, (UINT)(sizeof(Color) * _width), (UINT)(sizeof(Color) * _height), 0, nullptr);
		assert(SUCCEEDED(result));

		for (size_t i = 0; i < _dataCount; i++) {
			size_t posX = currentPos + (i % _width);
			size_t posY = i / _width;

			size_t access = (posY * textureWidth) + posX;

			imageData[access].r = _image[i].r;
			imageData[access].g = _image[i].g;
			imageData[access].b = _image[i].b;
			imageData[access].a = _image[i].a;
		}

		currentPos += _width;

		delete[] _image;
	}

	Texture texture = Texture();

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	RDirectX::GetInstance()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);

	texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * (UINT)textureWidth,
		sizeof(Color) * (UINT)imageDataCount
	);

	delete[] imageData;

	string _handle = handle;
	if (_handle.empty()) {
		_handle = "NoNameStringTextureHandle_" + text;
	}
	return TextureManager::Register(texture, _handle);
}

void TextDrawer::Init()
{
	HRESULT result;

	//デスクリプタヒープ(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = numSRVDescritors;

	//生成
	srvHeap = nullptr;
	result = RDirectX::GetInstance()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}
