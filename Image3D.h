#pragma once
#include "Obj3D.h"
#include "RConstBuffer.h"
#include "Material.h"
#include "Texture.h"

class Image3D final : public Obj3D
{
public:
	Texture* texture = nullptr;
	Material material;
	//TransformはObj3Dにある
	XMFLOAT2 size = { 1, 1 };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;

	Image3D() {};
	Image3D(Texture* texture, XMFLOAT2 size);

	//初期化処理
	void Init();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

