#include "TextDrawer.h"
#include "RDirectX.h"

using namespace std;

Texture TextDrawer::GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize)
{
	TextDrawer* drawer = GetInstance();

	if (glyph.length() != 1) {
		return Texture();
	}

	wstring _wGlyph = wstring(glyph.begin(), glyph.end());
	wstring _wTypeFace = wstring(fontTypeFace.begin(), fontTypeFace.end());

	Glyph glyphData = { _wTypeFace, _wGlyph };

	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };

	HFONT _font = CreateFont(fontSize, 0, 0, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"ＭＳ Ｐ明朝");

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, _font);
	UINT code = (UINT)*_wGlyph.c_str();

	OutputDebugString((_wGlyph + L"\n").c_str());

	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// オブジェクトの開放
	SelectObject(hdc, oldFont);
	DeleteObject(_font);
	ReleaseDC(NULL, hdc);

	int fontWidth = (GM.gmBlackBoxX + 3) / 4 * 4;
	int fontHeight = GM.gmBlackBoxY;
	int fontDataCount = fontWidth * fontHeight;

	OutputDebugString((wstring(L"fontWidth: ") + to_wstring(fontWidth) + L"(" + to_wstring(GM.gmBlackBoxX) + L")\n").c_str());
	OutputDebugString((wstring(L"fontHeight: ") + to_wstring(fontHeight) + L"\n").c_str());

	Color* imageData = new Color[fontDataCount];

	for (size_t i = 0; i < fontDataCount; i++) {
		float grayScale = (float)ptr[i];
		imageData[i].r = 1.0f;
		imageData[i].g = 1.0f;
		imageData[i].b = 1.0f;
		imageData[i].a = (grayScale * 255.0f / 16) / 255.0f;
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
	UINT incrementSize = RDirectX::GetInstance()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
