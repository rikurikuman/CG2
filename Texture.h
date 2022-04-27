#pragma once
#include <d3d12.h>
#include <string>
#include <list>
#include <wrl.h>
#include <memory>

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource; //�e�N�X�`���̃��\�[�X
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(CPU��)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(GPU��)
	std::string filePath; //�t�@�C���ւ̃p�X

	Texture() {};
	Texture(
		Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
		std::string filePath
	) : resource(resource), cpuHandle(cpuHandle), gpuHandle(gpuHandle), filePath(filePath) {};
};

class TextureManager
{
public:
	//TextureManager���擾����
	static TextureManager* GetInstance() {
		static TextureManager instance;
		return &instance;
	}

	/// <summary>
	/// �t�@�C������e�N�X�`����ǂݍ���
	/// </summary>
	/// <param name="filepath">�t�@�C���ւ̃p�X</param>
	/// <returns>�ǂݍ��񂾃e�N�X�`���ւ̃|�C���^</returns>
	static Texture* Load(const std::string filepath);

	//�ǂݍ��񂾃e�N�X�`����S�Ĕj������
	static void UnloadAll();

	ID3D12DescriptorHeap* GetSRVHeap() {
		return this->srvHeap.Get();
	}

private:
	TextureManager() {
		Init();
	};
	~TextureManager() = default;
	TextureManager(const TextureManager& a) {};
	TextureManager& operator=(const TextureManager&) { return *this; }

	void Init();

	static const size_t kNumSRVDescritors = 256; //�f�X�N���v�^�q�[�v�̐�
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; //�e�N�X�`���pSRV�f�X�N���v�^�q�[�v
	UINT nextIndex = 0;
	std::list<std::shared_ptr<Texture>> textures; //�e�N�X�`�����X�g
};