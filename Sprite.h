#pragma once
#include <d3d12.h>
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "RConstBuffer.h"
#include "ViewProjection.h"

class Sprite
{
public:
	Texture* texture = nullptr;
	Material material;
	Transform transform;
	XMFLOAT2 size = { 0, 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Sprite(Texture* texture);
	Sprite(Texture* texture, XMFLOAT2 size);

	void Init();

	//各データのバッファへの転送
	void TransferBuffer();

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands();
};

