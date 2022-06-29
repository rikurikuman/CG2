#include "ModelObj.h"

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	model->material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);
	viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::DrawCommands()
{
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &model->vertexBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&model->indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(model->material.texture).gpuHandle);

	//�`��R�}���h
	RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(static_cast<UINT>(model->indices.size()), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
