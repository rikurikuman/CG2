#pragma once
#include <DirectXMath.h>

struct ViewProjectionBuffer
{
	DirectX::XMMATRIX matrix;
};

class ViewProjection
{
public:
	DirectX::XMMATRIX matrix;
	DirectX::XMFLOAT3 eye = {0, 0, 0}; //���_���W
	DirectX::XMFLOAT3 target = {0, 0, 1}; //�����_���W
	DirectX::XMFLOAT3 up = {0, 1, 0}; //������x�N�g��

	float fov = DirectX::XMConvertToRadians(45); //��p(Rad)
	float aspect = 1; //�A�X�y�N�g��(��/�c)
	float nearclip = 0.1f; //�j�A�N���b�v����
	float farclip = 1000; //�t�@�[�N���b�v����

	//�����o�ϐ��̏��Ŋe�s��𐶐�����
	void UpdateMatrix();

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(ViewProjectionBuffer* target);
};