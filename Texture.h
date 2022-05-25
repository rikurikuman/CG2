#pragma once
#include <d3d12.h>
#include <string>
#include <list>
#include <wrl.h>
#include <memory>

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource; //テクスチャのリソース
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)
	std::string filePath; //ファイルへのパス

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
	//TextureManagerを取得する
	static TextureManager* GetInstance() {
		static TextureManager instance;
		return &instance;
	}

	static Texture* GetHogeHogeTexture();

	/// <summary>
	/// ファイルからテクスチャを読み込む
	/// </summary>
	/// <param name="filepath">ファイルへのパス</param>
	/// <returns>読み込んだテクスチャへのポインタ</returns>
	static Texture* Load(const std::string filepath);

	//読み込んだテクスチャを全て破棄する
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

	static const size_t kNumSRVDescritors = 256; //デスクリプタヒープの数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; //テクスチャ用SRVデスクリプタヒープ
	UINT nextIndex = 0;
	std::list<std::shared_ptr<Texture>> textures; //テクスチャリスト
};