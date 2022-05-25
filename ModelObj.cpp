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
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &model->vertexBuff.view);

	//インデックスバッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetIndexBuffer(&model->indexBuff.view);

	//定数バッファビューの設定コマンド
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	if (model->material.texture != nullptr) {
		GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, model->material.texture->gpuHandle);
	}
	else {
		GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::GetHogeHogeTexture()->gpuHandle);
	}

	//描画コマンド
	GetRDirectX()->cmdList->DrawIndexedInstanced(static_cast<UINT>(model->indices.size()), 1, 0, 0, 0); // 全ての頂点を使って描画
}
