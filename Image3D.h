#pragma once
#include "Obj3D.h"
#include "RConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Image3D final : public Obj3D
{
public:
	TextureHandle texture;
	Material material;
	//Transform��Obj3D�ɂ���
	Vector2 size = { 1, 1 };

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};
	Image3D(TextureHandle texture, Vector2 size);

	//����������
	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

