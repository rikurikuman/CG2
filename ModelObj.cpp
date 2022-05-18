#include "ModelObj.h"

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);
	viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::DrawCommands()
{
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &model->vertexBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetIndexBuffer(&model->indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//�`��R�}���h
	GetRDirectX()->cmdList->DrawIndexedInstanced(static_cast<UINT>(model->indices.size()), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
