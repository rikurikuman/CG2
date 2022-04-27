#pragma once
#include <DirectXMath.h>

struct MaterialBuffer {
	DirectX::XMFLOAT4 color;
};

class Material
{
public:
	DirectX::XMFLOAT4 color = {1, 1, 1, 1}; //色(RGBA)

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(MaterialBuffer* target);
};