#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite(Texture* texture)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	this->size.x = texture->resource->GetDesc().Width;
	this->size.y = texture->resource->GetDesc().Height;

	Init();
}

Sprite::Sprite(Texture* texture, Vector2 size)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}

void Sprite::Init()
{
	//���_�f�[�^
	Vertex vertices[] = {
		{{ -0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //����
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //����
		{{ 0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //�E��
		{{ 0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
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
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vertBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetIndexBuffer(&indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����(�w�i)
	GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, texture->gpuHandle);

	TransferBuffer();

	//�`��R�}���h
	GetRDirectX()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void SpriteManager::Init()
{
	rootSignature = GetRDirectX()->rootSignature;

	//Basic�V�F�[�_�B�̓ǂݍ��݂ƃR���p�C��
	ComPtr<ID3DBlob> errorBlob = nullptr; //�G���[�I�u�W�F�N�g

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// �V�F�[�_�[�̐ݒ�
	pipelineState.desc.VS.pShaderBytecode = GetRDirectX()->basicVS.shaderBlob->GetBufferPointer();
	pipelineState.desc.VS.BytecodeLength = GetRDirectX()->basicVS.shaderBlob->GetBufferSize();
	pipelineState.desc.PS.pShaderBytecode = GetRDirectX()->basicPS.shaderBlob->GetBufferPointer();
	pipelineState.desc.PS.BytecodeLength = GetRDirectX()->basicPS.shaderBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineState.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W��

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	// �u�����h�X�e�[�g(����������)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineState.desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// ���_���C�A�E�g�̐ݒ�
	pipelineState.desc.InputLayout.pInputElementDescs = inputLayout;
	pipelineState.desc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineState.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineState.desc.NumRenderTargets = 1;
	pipelineState.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineState.desc.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

	//�Z�b�g
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}
