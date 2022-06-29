#include "Image3D.h"
#include "RDirectX.h"
#include "Vertex.h"

Image3D::Image3D(TextureHandle texture, Vector2 size)
{
	this->texture = texture;
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}
	
void Image3D::Init()
{
	//頂点データ
	VertexPNU vertices[] = {
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f * size.x,  0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{  0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{  0.5f * size.x,  0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
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

void Image3D::TransferBuffer(ViewProjection viewprojection)
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);
	viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void Image3D::DrawCommands()
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

	//描画コマンド
	RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}
