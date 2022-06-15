#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite(Texture* texture)
{
	this->texture = texture;

	//サイズをセットする
	this->size.x = texture->resource->GetDesc().Width;
	this->size.y = texture->resource->GetDesc().Height;

	Init();
}

Sprite::Sprite(Texture* texture, Vector2 size)
{
	this->texture = texture;

	//サイズをセットする
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}

void Sprite::Init()
{
	//頂点データ
	Vertex vertices[] = {
		{{ -0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{ 0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{ 0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	Vertex::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, GetRWindow()->GetWidth(),
		0.0f, GetRWindow()->GetHeight(),
		0.0f, 1.0f
	);
	
	viewProjectionBuff.constMap->matrix = matProjection;
}

void Sprite::DrawCommands()
{
	//頂点バッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vertBuff.view);

	//インデックスバッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetIndexBuffer(&indexBuff.view);

	//定数バッファビューの設定コマンド
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, texture->gpuHandle);

	TransferBuffer();

	//描画コマンド
	GetRDirectX()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void SpriteManager::Init()
{
	rootSignature = GetRDirectX()->rootSignature;

	pipelineState = GetRDirectX()->pipelineState;

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	pipelineState.Create();
}
