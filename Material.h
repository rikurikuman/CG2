#pragma once
#include "Color.h"

struct MaterialBuffer {
	Color color;
};

class Material
{
public:
	Color color = {1, 1, 1, 1}; //�F(RGBA)

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(MaterialBuffer* target);
};