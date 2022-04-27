#include "Texture.h"
#include "RDirectX.h"
#include <DirectXTex.h>

using namespace std;
using namespace DirectX;

void TextureManager::Init()
{
	HRESULT result;

	//デスクリプタヒープ(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = kNumSRVDescritors;

	//生成
	srvHeap = nullptr;
	result = GetRDirectX()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

Texture* TextureManager::Load(const string filepath)
{
	TextureManager* manager = TextureManager::GetInstance();
	HRESULT result;

	//一回読み込んだことがあるファイルはそのまま返す
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

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath,
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	assert(SUCCEEDED(result));

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = GetRDirectX()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture->resource)
	);
	assert(SUCCEEDED(result));

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture->resource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	//シェーダーリソースビュー
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = manager->srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = manager->srvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT incrementSize = GetRDirectX()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += manager->nextIndex * incrementSize;
	_gpuHandle.ptr += manager->nextIndex * incrementSize;
	texture->cpuHandle = _cpuHandle;
	texture->gpuHandle = _gpuHandle;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//生成
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