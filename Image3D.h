#pragma once
#include "Obj3D.h"
#include "RConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"

class Image3D final : public Obj3D
{
public:
	Texture* texture = nullptr;
	Material material;
	//Transform��Obj3D�ɂ���
	Vector2 size = { 1, 1 };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjection> viewProjectionBuff;

	Image3D() {};
	Image3D(Texture* texture, Vector2 size);

	//����������
	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

