#pragma once
#include "Obj3D.h"
#include "Image3D.h"
class Cube : public Obj3D
{
public:
	enum Direction {
		Front, Left, Back, Right, Top, Bottom
	};

	Image3D faces[6];
	XMFLOAT2 size = { 1, 1 };

	Cube(XMFLOAT2 size = {1, 1});
	Cube(Texture* texture, XMFLOAT2 size);

	/// <summary>
	/// �w�肵���ʂ̃e�N�X�`�����Z�b�g����
	/// </summary>
	/// <param name="texture">�e�N�X�`���̃|�C���^</param>
	/// <param name="direction">�ʂ̌���</param>
	void SetTexture(Texture* texture, Direction direction);

	/// <summary>
	/// �S�Ă̖ʂ̃e�N�X�`�����Z�b�g����
	/// </summary>
	/// <param name="texture"></param>
	void SetAllTexture(Texture* texture);

	//�ʂ�Transform�Ƃ����X�V����
	void UpdateFaces();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

