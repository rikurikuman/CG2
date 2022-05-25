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
	Texture* texture = nullptr;
	Material material;
	//TransformはObj3Dにある
	Vector2 size = { 1, 1 };

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};
	Image3D(Texture* texture, Vector2 size);

	//初期化処理
	void Init();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

