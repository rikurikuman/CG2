#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct TransformBuffer
{
	XMMATRIX matrix;
};

class Transform
{
public:
	Transform* parent = nullptr;
	XMFLOAT3 position = { 0, 0, 0 };
	XMFLOAT3 rotation = { 0, 0, 0 };
	XMFLOAT3 scale = { 1, 1, 1 };

	XMMATRIX matrix;

	//pos(0, 0, 0), rot(0, 0, 0), scale(1, 1, 1)��Transform�𐶐�
	Transform() {
		UpdateMatrix();
	}

	Transform(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale) : position(position), rotation(rotation), scale(scale) {
		UpdateMatrix();
	}

	//�����o�ϐ���3���Ń��[���h���W�ϊ��s��𐶐�����
	void UpdateMatrix();

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(TransformBuffer* target);

	/// <summary>
	/// �s��Ɗ|���Z���Ă���萔�o�b�t�@�ɓ]��,
	/// �r���[+�ˉe�ϊ��s������Ċy����p
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	/// <param name="matrix">�]���O�Ɋ|����s��</param>
	void Transfer(TransformBuffer* target, XMMATRIX matrix);
};

