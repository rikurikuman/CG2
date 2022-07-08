#pragma once
#include "Texture.h"

class TextDrawer
{
public:
	static TextDrawer* GetInstance() {
		static TextDrawer instance;
		return &instance;
	}

	static Texture GetFontTexture(std::string glyph, std::string fontTypeFace, UINT fontSize, bool useAlign = false);
	static Texture GetFontTexture(std::wstring glyph, std::wstring fontTypeFace, UINT fontSize, bool useAlign = false);

	static TextureHandle CreateStringTexture(std::string text, std::string fontTypeFace, UINT fontSize, std::string handle = "");

	//static void DrawString(int x, int y, std::string text, std::string fontTypeFace, UINT fontSize);

private:
	struct Glyph {
		std::wstring glyph;
		std::wstring fontTypeFace;
		UINT fontSize;

		bool operator==(const Glyph& a) const {
			return glyph == a.glyph && fontTypeFace == a.fontTypeFace && fontSize == a.fontSize;
		}

		bool operator<(const Glyph& a) const {
			if (glyph < a.glyph) {
				return true;
			}
			else if (glyph == a.glyph) {
				if (fontTypeFace < a.fontTypeFace) {
					return true;
				}
				else if (fontTypeFace == a.fontTypeFace) {
					return fontSize < a.fontSize;
				}
			}

			return false;
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

