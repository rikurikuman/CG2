#pragma once
#include <DirectXMath.h>

struct MaterialBuffer {
	DirectX::XMFLOAT4 color;
};

class Material
{
public:
	DirectX::XMFLOAT4 color = {1, 1, 1, 1}; //�F(RGBA)

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(MaterialBuffer* target);
};