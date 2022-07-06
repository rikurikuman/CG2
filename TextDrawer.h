#pragma once
#include "Texture.h"

class TextDrawer
{
public:
	static TextDrawer* GetInstance() {
		static TextDrawer instance;
		return &instance;
	}

	static Texture GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize);

	static void DrawString(int x, int y, std::string text, std::string fontTypeFace, UINT fontSize);

private:
	struct Glyph {
		std::wstring fontTypeFace;
		std::wstring glyph;

		bool operator<(const Glyph& a) const {
			if (fontTypeFace < a.fontTypeFace) {
				return true;
			}
			return glyph < a.glyph;
		}
	};

	TextDrawer() {
		Init();
	};
	~TextDrawer() = default;
	TextDrawer(const TextDrawer& a) {};
	TextDrawer& operator=(const TextDrawer&) { return *this; }

	void Init();

	static const UINT numSRVDescritors = 2048; //デスクリプタヒープの数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; //グリフ用SRVデスクリプタヒープ
	UINT nextIndex = 0;
	std::map<Glyph, Texture> glyphMap; //グリフテクスチャのマップ
};

