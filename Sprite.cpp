#include "Sprite.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;
using namespace DirectX;

Sprite::Sprite(Texture* texture)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	this->size.x = texture->resource->GetDesc().Width;
	this->size.y = texture->resource->GetDesc().Height;

	Init();
}

Sprite::Sprite(Texture* texture, XMFLOAT2 size)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}

void Sprite::Init()
{
	HRESULT result;

	//�ȉ��`��f�[�^

	//���_�f�[�^
	Vertex vertices[] = {
		{{ -0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //����
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //����
		{{ 0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //�E��
		{{ 0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	uint16_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	Vertex::CalcNormalVec(vertices, indices, _countof(indices));

	//���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�o�b�t�@���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�C���f�b�N�X�o�b�t�@����
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	vbView.SizeInBytes = sizeVB; //���_�o�b�t�@�̃T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]); //���_��̃T�C�Y

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Sprite::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);

	XMMATRIX matProjection = XMMatrixOrthographicOffCenterLH(
		0, GetRWindow()->GetWidth(),
		GetRWindow()->GetHeight(), 0,
		0, 1
	);
	
	viewProjectionBuff.constMap->matrix = matProjection;
}

void Sprite::DrawCommands()
{
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	GetRDirectX()->cmdList->IASetIndexBuffer(&ibView);

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
	// �f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam[4] = {};
	//�e�N�X�`�����W�X�^0��
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParam[0].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1; //�f�X�N���v�^�����W��
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@0��(Material)
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParam[1].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParam[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@1��(Transform)
	rootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParam[2].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParam[2].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@2��(Transform)
	rootParam[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParam[3].Descriptor.ShaderRegister = 2; //�萔�o�b�t�@�ԍ�
	rootParam[3].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParam[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	// �e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������

	// ���[�g�V�O�l�`���̐ݒ�
	rootSignature.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignature.desc.pParameters = rootParam;
	rootSignature.desc.NumParameters = _countof(rootParam); //�p�����[�^��
	rootSignature.desc.pStaticSamplers = &samplerDesc;
	rootSignature.desc.NumStaticSamplers = 1;

	rootSignature.Create();

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
	pipelineState.desc.VS.pShaderBytecode = GetRDirectX()->basicVSBlob->GetBufferPointer();
	pipelineState.desc.VS.BytecodeLength = GetRDirectX()->basicVSBlob->GetBufferSize();
	pipelineState.desc.PS.pShaderBytecode = GetRDirectX()->basicPSBlob->GetBufferPointer();
	pipelineState.desc.PS.BytecodeLength = GetRDirectX()->basicPSBlob->GetBufferSize();

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
