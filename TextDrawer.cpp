#include "TextDrawer.h"
#include "RDirectX.h"
#include "Util.h"

using namespace std;

FontTexture TextDrawer::GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize, bool useAlign)
{
	wstring wGlyph = Util::ConvertStringToWString(glyph);
	wstring wFontTypeFace = Util::ConvertStringToWString(fontTypeFace);
	return GetFontTexture(wGlyph, wFontTypeFace, fontSize, useAlign);
}

FontTexture TextDrawer::GetFontTexture(std::wstring glyph, std::wstring fontTypeFace, UINT fontSize, bool useAlign)
{
	TextDrawer* drawer = GetInstance();

	if (glyph.length() != 1) {
		return FontTexture();
	}

	Glyph glyphData = { glyph, fontTypeFace, fontSize };

	auto itr = drawer->glyphMap.find(glyphData);
	if (itr != drawer->glyphMap.end()) {
		return itr->second;
	}

	TEXTMETRIC tm;
	GLYPHMETRICS gm;
	CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };

	HFONT _font = CreateFont(fontSize, 0, 0, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, fontTypeFace.c_str());

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, _font);
	UINT code = (UINT)*glyph.c_str();

	OutputDebugString((glyph + L"\n").c_str());

	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetTextMetrics(hdc, &tm);
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, size, ptr, &Mat);

	// オブジェクトの開放
	SelectObject(hdc, oldFont);
	DeleteObject(_font);
	ReleaseDC(NULL, hdc);

	int fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
	int fontHeight = size / fontWidth;

	int baseLineY = tm.tmAscent;
	int glyphOriginY = 0;

	if (useAlign) {
		glyphOriginY = baseLineY - gm.gmptGlyphOrigin.y;
		fontHeight = glyphOriginY + fontHeight;
	}

	int fontDataCount = fontWidth * fontHeight;

	OutputDebugString((wstring(L"fontWidth: ") + to_wstring(fontWidth) + L"(" + to_wstring(gm.gmBlackBoxX) + L")\n").c_str());
	OutputDebugString((wstring(L"fontHeight: ") + to_wstring(fontHeight) + L"(" + to_wstring(size / fontWidth) + L")\n").c_str());

	Color* imageData = new Color[fontDataCount];
	for (size_t i = 0; i < fontDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	for (size_t i = 0; i < size; i++) {
		size_t posX = i % fontWidth;
		size_t posY = glyphOriginY + (i / fontWidth);

		size_t access = (posY * fontWidth) + posX;
		assert(access < fontDataCount);

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

	delete[] ptr;
	delete[] imageData;

	FontTexture ftex;
	ftex.texture = texture;
	ftex.tm = tm;
	ftex.gm = gm;

	drawer->glyphMap[glyphData] = ftex;

	return ftex;
}

TextureHandle TextDrawer::CreateStringTexture(std::string text, std::string fontTypeFace, UINT fontSize, std::string handle)
{
	wstring wText = Util::ConvertStringToWString(text);
	wstring _wTypeFace = Util::ConvertStringToWString(fontTypeFace);

	list<FontTexture> glyphlist;

	for (size_t i = 0; i < wText.size(); i++) {
		glyphlist.push_back(GetFontTexture(wText.substr(i, 1), _wTypeFace, fontSize, true));
	}

	UINT64 originPos = 0;
	UINT64 textureWidth = 0;
	UINT textureHeight = 0;

	for (FontTexture fTex : glyphlist) {
		originPos += fTex.gm.gmptGlyphOrigin.x;
		textureWidth += fTex.texture.resource->GetDesc().Width;
		originPos += fTex.gm.gmCellIncX;
		if (textureWidth < originPos) {
			textureWidth += originPos - textureWidth;
		}

		UINT height = fTex.texture.resource->GetDesc().Height;
		if (textureHeight < height) {
			textureHeight = height;
		}
	}

	UINT64 imageDataCount = textureWidth * textureHeight;

	Color* imageData = new Color[imageDataCount];
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	UINT64 currentPos = 0; //現在の原点位置

	for (FontTexture tex : glyphlist) {
		size_t _width = tex.texture.resource->GetDesc().Width;
		size_t _height = tex.texture.resource->GetDesc().Height;
		size_t _dataCount = _width * _height;

		Color* _image = new Color[_dataCount];

		HRESULT result = tex.texture.resource->ReadFromSubresource(_image, (UINT)(sizeof(Color) * _width), (UINT)(sizeof(Color) * _height), 0, nullptr);
		assert(SUCCEEDED(result));

		for (size_t i = 0; i < _dataCount; i++) {
			size_t posX = currentPos + tex.gm.gmptGlyphOrigin.x + (i % _width);
			size_t posY = i / _width;

			size_t access = (posY * textureWidth) + posX;
			assert(access < imageDataCount);

			imageData[access].r = _image[i].r;
			imageData[access].g = _image[i].g;
			imageData[access].b = _image[i].b;
			imageData[access].a = _image[i].a;
		}

		currentPos += tex.gm.gmCellIncX;

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
}
