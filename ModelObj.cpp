#include "ModelObj.h"

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	model->material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);
	viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::DrawCommands()
{
	//頂点バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &model->vertexBuff.view);

	//インデックスバッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&model->indexBuff.view);

	//定数バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(model->material.texture).gpuHandle);

	//描画コマンド
	RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(static_cast<UINT>(model->indices.size()), 1, 0, 0, 0); // 全ての頂点を使って描画
}
