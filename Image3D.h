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
private:
	TextureHandle texture;
	Vector2 scale = { 1, 1 };
	Vector2 size = { 1, 1 };
	bool forceSize = false;

public:
	Material material;
	//TransformはObj3Dにある

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle tex) {
		texture = tex;
		if (!forceSize) {
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * size.x;
			this->size.y = size.y;
		}
	}

	TextureHandle GetTexture() {
		return texture;
	}

	void SetSize(Vector2 size, bool forceSize = false) {
		this->forceSize = forceSize;
		if (forceSize) {
			this->size = size;
		}
		else {
			scale = size;
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * scale.x;
			this->size.y = scale.y;
		}
	}

	Vector2 GetSize() {
		return size;
	}

	//初期化処理
	void Init();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

