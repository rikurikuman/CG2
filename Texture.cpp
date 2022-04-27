#include "Texture.h"
#include "RDirectX.h"
#include <DirectXTex.h>

using namespace std;
using namespace DirectX;

void TextureManager::Init()
{
	HRESULT result;

	//�f�X�N���v�^�q�[�v(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�V�F�[�_�[���猩����
	srvHeapDesc.NumDescriptors = kNumSRVDescritors;

	//����
	srvHeap = nullptr;
	result = GetRDirectX()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

Texture* TextureManager::Load(const string filepath)
{
	TextureManager* manager = TextureManager::GetInstance();
	HRESULT result;

	//���ǂݍ��񂾂��Ƃ�����t�@�C���͂��̂܂ܕԂ�
	auto itr = find_if(manager->textures.begin(), manager->textures.end(), [&](const shared_ptr<Texture> texture) {
		return texture->filePath == filepath;
	});
	if (itr != manager->textures.end()) {
		return itr->get();
	}

	shared_ptr<Texture> texture = shared_ptr<Texture>(new Texture());
	texture->filePath = filepath;
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilePath, _countof(wfilePath));

	// �摜�C���[�W�f�[�^
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		wfilePath,
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
	result = GetRDirectX()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture->resource)
	);
	assert(SUCCEEDED(result));

	//�Ă񂻁[
	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture->resource->WriteToSubresource(
			(UINT)i,
			nullptr, //�S�̈�փR�s�[
			img->pixels, //���f�[�^�A�h���X
			(UINT)img->rowPitch, //1���C���T�C�Y
			(UINT)img->slicePitch //1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	//�V�F�[�_�[���\�[�X�r���[
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = manager->srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = manager->srvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT incrementSize = GetRDirectX()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += manager->nextIndex * incrementSize;
	_gpuHandle.ptr += manager->nextIndex * incrementSize;
	texture->cpuHandle = _cpuHandle;
	texture->gpuHandle = _gpuHandle;

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//����
	GetRDirectX()->device->CreateShaderResourceView(texture->resource.Get(), &srvDesc, _cpuHandle);

	manager->nextIndex++;

	manager->textures.push_back(texture);
	return texture.get();
}

void TextureManager::UnloadAll()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->Init();

	manager->textures.clear();
}