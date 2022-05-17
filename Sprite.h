#pragma once
#include <d3d12.h>
#include "Vector2.h"
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
	Vector2 size = { 0, 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Sprite(Texture* texture);
	Sprite(Texture* texture, Vector2 size);

	void Init();

	//各データのバッファへの転送
	void TransferBuffer();

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands();
};

class SpriteManager
{
public:
	static SpriteManager* GetInstance() {
		static SpriteManager instance;
		return &instance;
	}

	RootSignature GetRootSignature() {
		return rootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return pipelineState;
	}

private:
	SpriteManager() {
		Init();
	};
	~SpriteManager() = default;
	SpriteManager(const SpriteManager& a) {};
	SpriteManager& operator=(const SpriteManager&) { return *this; }

	void Init();

	RootSignature rootSignature;
	GraphicsPipeline pipelineState;
};

