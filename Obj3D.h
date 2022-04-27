#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* parent = nullptr;

public:
	Transform transform;

	void SetParent(Obj3D* parent) {
		this->parent = parent;
		this->transform.parent = &parent->transform;
	}

	const Obj3D* GetParent() {
		return parent;
	}

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	virtual void DrawCommands() {}
};
