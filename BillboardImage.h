#pragma once
#include "Obj3D.h"
#include "Texture.h"
#include "Vector2.h"
#include "Image3D.h"

class BillboardImage : public Obj3D
{
private:
	bool billboardX = false;
	bool billboardY = false;
	bool billboardZ = false;

public:
	Image3D image;

	BillboardImage() {};
	BillboardImage(Texture* texture, Vector2 size)
	{
		Init(texture, size);
	}

	//����������
	void Init(Texture* texture, Vector2 size);

	void Update(const ViewProjection& vp);

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

