#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"
#include <DirectXTex.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifdef _DEBUG
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif

	//WindowsAPI
	InitRWindow();

	//DirectX
	InitRDirectX();

	//DirectInput
	InitInput();

	HRESULT result;

	//�ȉ��`��f�[�^
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	//���_�f�[�^
	Vertex vertices[] = {
		{{ -50.0f, -50.0f, 0.0f }, {0.0f, 1.0f}}, //����
		{{ -50.0f, +50.0f, 0.0f }, {0.0f, 0.0f}}, //����
		{{ +50.0f, -50.0f, 0.0f }, {1.0f, 1.0f}}, //�E��
		{{ +50.0f, +50.0f, 0.0f }, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	uint16_t indices[] = {
		0, 1, 2,
		1, 2, 3
	};

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
	ComPtr<ID3D12Resource> vertBuff = nullptr;
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
	ComPtr<ID3D12Resource> indexBuff = nullptr;
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
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	vbView.SizeInBytes = sizeVB; //���_�o�b�t�@�̃T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]); //���_��̃T�C�Y

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ComPtr<ID3DBlob> vsBlob = nullptr; //���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr; //�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr; //�G���[�I�u�W�F�N�g

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", //�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0", //�G���g�����A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	result = D3DCompileFromFile(
		L"BasicPS.hlsl", //�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0", //�G���g�����A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W��

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.DepthClipEnable = true;

	// �u�����h�X�e�[�g(����������)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	pipelineDesc.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

	// �萔�o�b�t�@
	struct ConstBufferDataMaterial {
		XMFLOAT4 color; //�F(RGBA)
	};

	struct ConstBufferDataTransform {
		XMMATRIX matrix; //3D�ϊ��s��
	};

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	// �萔�o�b�t�@�̐���
	result = GetRDirectX()->device->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	ComPtr<ID3D12Resource> constBuffTransform = nullptr;
	{
		//transform�p
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		// ���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// �萔�o�b�t�@�̐���
		result = GetRDirectX()->device->CreateCommittedResource(
			&cbHeapProp, //�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc, //���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform)
		);
		assert(SUCCEEDED(result));
	}	

	// �萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); //�}�b�s���O
	assert(SUCCEEDED(result));
	ConstBufferDataTransform* constMapTransform = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform); //�}�b�s���O
	assert(SUCCEEDED(result));

	// �摜�C���[�W�f�[�^
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/conflict.jpg",
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	assert(SUCCEEDED(result));

	// �~�b�v�}�b�v����
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}

	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// �e�N�X�`���o�b�t�@
	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//����
	ComPtr<ID3D12Resource> texBuff = nullptr;
	result = GetRDirectX()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
	assert(SUCCEEDED(result));

	//�Ă񂻁[
	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr, //�S�̈�փR�s�[
			img->pixels, //���f�[�^�A�h���X
			(UINT)img->rowPitch, //1���C���T�C�Y
			(UINT)img->slicePitch //1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	//�V�F�[�_�[���\�[�X�r���[
	const size_t kMaxSRVCount = 2056; //�V�F�[�_�[���\�[�X�r���[�̍ő��

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�V�F�[�_�[���猩����
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//����
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;
	result = GetRDirectX()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//����
	GetRDirectX()->device->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam[3] = {};
	//�萔�o�b�t�@0��(Material)
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParam[0].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParam[0].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParam[1].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1; //�f�X�N���v�^�����W��
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@1��(Transform)
	rootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParam[2].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParam[2].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	//�e�N�X�`���T���v���[�̐ݒ�
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

	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParam;
	rootSignatureDesc.NumParameters = _countof(rootParam); //�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = GetRDirectX()->device->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//�Z�b�g
	pipelineDesc.pRootSignature = rootSignature.Get();

	// �p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = GetRDirectX()->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//���낢��
	XMFLOAT3 pos = { 0, 0, 0 };
	XMFLOAT3 scale = {1, 1, 1};
	XMFLOAT3 rot = {0, 0, 0};
	float angle = 0.0f; //�J�����̉�]�p

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45),
		(float)WIN_WIDTH / WIN_HEIGHT,
		0.1f, 1000.0f
	);

	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100); //���_���W
	XMFLOAT3 target(0, 0, 0); //�����_���W
	XMFLOAT3 up(0, 1, 0); //������x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	while (true) {
		GetRWindow()->ProcessMessage();

		if (GetRWindow()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		UpdateInput();

		if (GetKey(DIK_1)) {
			constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);
		}
		else {
			constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);
		}

		/*constMapTransform->matrix = XMMatrixOrthographicOffCenterLH(
			0, WIN_WIDTH,
			WIN_HEIGHT, 0,
			0, 1
		);*/

		if (GetKey(DIK_UP)) {
			pos.z += 1.0f;
		}
		if (GetKey(DIK_DOWN)) {
			pos.z -= 1.0f;
		}
		if (GetKey(DIK_RIGHT)) {
			pos.x += 1.0f;
		}
		if (GetKey(DIK_LEFT)) {
			pos.x -= 1.0f;
		}

		if (GetKey(DIK_D) || GetKey(DIK_A)) {
			if (GetKey(DIK_D)) { angle += XMConvertToRadians(1.0f); }
			else if (GetKey(DIK_A)) { angle += XMConvertToRadians(-1.0f); }

			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		XMMATRIX matWorld = XMMatrixIdentity();

		XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
		
		XMMATRIX matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(rot.z);
		matRot *= XMMatrixRotationX(rot.x);
		matRot *= XMMatrixRotationY(rot.y);

		XMMATRIX matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);

		matWorld *= matScale;
		matWorld *= matRot;
		matWorld *= matTrans;

		constMapTransform->matrix = matWorld * matView * matProjection;

		//�ȉ��`��
		//�o�b�N�o�b�t�@�ԍ��̎擾
		UINT bbIndex = GetRDirectX()->swapChain->GetCurrentBackBufferIndex();

		//���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = GetRDirectX()->backBuffers[bbIndex].Get();
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:�\������
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:�`���
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		//�o�b�N�o�b�t�@��`���ɂ���
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRDirectX()->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * GetRDirectX()->device->GetDescriptorHandleIncrementSize(GetRDirectX()->rtvHeap->GetDesc().Type);
		GetRDirectX()->cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//��ʃN���A�`
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; //���ۂ��F�ŃN���A����
		if (GetKey(DIK_SPACE)) {
			//�Ԃ��ۂ��F�ŃN���A����
			clearColor[0] = 0.5f;
			clearColor[1] = 0.1f;
			clearColor[2] = 0.1f;
			clearColor[3] = 0.0f;
		}
		GetRDirectX()->cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//�`��R�}���h
		//�r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = WIN_WIDTH;
		viewport.Height = WIN_HEIGHT;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		GetRDirectX()->cmdList->RSSetViewports(1, &viewport);

		//�V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + WIN_WIDTH;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + WIN_HEIGHT;
		GetRDirectX()->cmdList->RSSetScissorRects(1, &scissorRect);

		GetRDirectX()->cmdList->SetPipelineState(pipelineState.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(rootSignature.Get());

		//�v���~�e�B�u�`��ݒ�
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���_�o�b�t�@�r���[�̐ݒ�R�}���h
		GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vbView);

		//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		GetRDirectX()->cmdList->IASetIndexBuffer(&ibView);

		//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
		GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//SRV�q�[�v�̐ݒ�R�}���h
		ID3D12DescriptorHeap* _heap = srvHeap.Get();
		GetRDirectX()->cmdList->SetDescriptorHeaps(1, &_heap);
		//SRV�q�[�v�̐擪�n���h�����擾
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
		GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
		GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		//�`��R�}���h
		GetRDirectX()->cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��

		//���\�[�X�o���A��\���ɖ߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:�`�悩��
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:�\����
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		EndFrame();
	}
	return 0;
}