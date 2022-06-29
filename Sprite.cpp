#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite(TextureHandle texture, Vector2 anchor)
{
	this->texture = texture;

	//サイズをセットする
	
	this->size.x = (float)TextureManager::Get(texture).resource->GetDesc().Width;
	this->size.y = (float)TextureManager::Get(texture).resource->GetDesc().Height;

	//アンカーポイントをセットする
	this->anchor = anchor;

	Init();
}

void Sprite::Init()
{
	//頂点データ
	VertexPNU vertices[] = {
		{{ -anchor.x * size.x, (1 - anchor.y) * size.y, 0.0f}, {}, {0.0f, 1.0f}}, //左下
		{{ -anchor.x * size.x, -anchor.y * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{ (1 - anchor.x) * size.x, (1 - anchor.y) * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{ (1 - anchor.x) * size.x, -anchor.y * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);
	
	viewProjectionBuff.constMap->matrix = matProjection;
}

void Sprite::DrawCommands()
{
	//頂点バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuff.view);

	//インデックスバッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&indexBuff.view);

	//定数バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	TransferBuffer();

	//描画コマンド
	RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void SpriteManager::Init()
{
	rootSignature = RDirectX::GetInstance()->rootSignature;

	pipelineState = RDirectX::GetInstance()->pipelineState;

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	pipelineState.Create();
}
