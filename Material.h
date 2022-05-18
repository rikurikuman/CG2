#pragma once
#include "Color.h"

struct MaterialBuffer {
	Color color;
};

class Material
{
public:
	Color color = {1, 1, 1, 1}; //色(RGBA)

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(MaterialBuffer* target);
};