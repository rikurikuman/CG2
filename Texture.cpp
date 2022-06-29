#include "Texture.h"
#include "RDirectX.h"
#include <DirectXTex.h>
#include "Color.h"

using namespace std;
using namespace DirectX;

void TextureManager::Init()
{
	HRESULT result;

	//デスクリプタヒープ(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = numSRVDescritors;

	//生成
	srvHeap = nullptr;
	result = RDirectX::GetInstance()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	RegisterInternal(GetHogeHogeTexture(), "PreRegisteredTex_HogeHoge");
}

Texture TextureManager::GetHogeHogeTexture()
{
	HRESULT result;

	Texture texture = Texture();

	const size_t textureWidth = 256;
	const size_t textureHeight = 256;
	const size_t imageDataCount = textureWidth * textureHeight;
	Color* imageData = new Color[imageDataCount];

	for (size_t i = 0; i < imageDataCount; i++) {
		size_t x = i % textureWidth;
		size_t y = i / textureWidth;
		
		if ((x < textureWidth / 2 && y < textureHeight / 2)
			|| (x >= textureWidth / 2 && y >= textureHeight / 2)) {
			imageData[i] = Color(1, 0, 1, 1);
		}
		else {
			imageData[i] = Color(0, 0, 0, 1);
		}
	}

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
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetInstance()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	assert(SUCCEEDED(result));

	result = texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * textureWidth,
		sizeof(Color) * imageDataCount
	);

	delete imageData;
	return texture;
}

TextureHandle TextureManager::LoadInternal(const std::string filepath)
{
	HRESULT result;

	//一回読み込んだことがあるファイルはそのまま返す
	auto itr = find_if(textureMap.begin(), textureMap.end(), [&](const std::pair<TextureHandle, Texture> p) {
		return p.second.filePath == filepath;
		});
	if (itr != textureMap.end()) {
		return itr->first;
	}

	Texture texture = Texture();
	texture.filePath = filepath;
	wstring wfilePath(filepath.begin(), filepath.end());

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	if (FAILED(result)) {
		return "";
	}

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
	else {
		return "";
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
	result = RDirectX::GetInstance()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	if (FAILED(result)) {
		return "";
	}

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		if (FAILED(result)) {
			return "";
		}
	}

	return RegisterInternal(texture);
}

Texture& TextureManager::GetInternal(const TextureHandle& handle)
{
	auto itr = textureMap.find(handle);
	if (itr != textureMap.end()) {
		return itr->second;
	}
	return textureMap["PreRegisteredTex_HogeHoge"];
}

TextureHandle TextureManager::RegisterInternal(Texture& texture, TextureHandle handle)
{
	UINT useIndex = -1;
	for (UINT i = 0; i < TextureManager::numSRVDescritors; i++) {
		bool ok = true;
		for (std::pair<TextureHandle, Texture> p : textureMap) {
			if (p.second.heapIndex == i) {
				ok = false;
				break;
			}
		}

		if (ok) {
			useIndex = i;
			break;
		}
	}

	if (useIndex == -1) {
		return TextureHandle();
	}

	//シェーダーリソースビュー
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT incrementSize = RDirectX::GetInstance()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += useIndex * incrementSize;
	_gpuHandle.ptr += useIndex * incrementSize;
	texture.cpuHandle = _cpuHandle;
	texture.gpuHandle = _gpuHandle;
	texture.heapIndex = useIndex;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.resource->GetDesc().Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture.resource->GetDesc().MipLevels;

	//生成
	RDirectX::GetInstance()->device->CreateShaderResourceView(texture.resource.Get(), &srvDesc, _cpuHandle);

	if (handle.empty()) {
		handle = "NoNameHandle_" + useIndex;
	}

	textureMap[handle] = texture;
	return handle;
}

void TextureManager::UnRegisterInternal(const TextureHandle& handle)
{
	textureMap.erase(handle);
}

TextureHandle TextureManager::Load(const string filepath)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->LoadInternal(filepath);
}

Texture& TextureManager::Get(const TextureHandle& handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	auto itr = manager->textureMap.find(handle);
	if (itr != manager->textureMap.end()) {
		return itr->second;
	}
	return manager->textureMap["PreRegisteredTex_HogeHoge"];
}

TextureHandle TextureManager::Register(Texture& texture, TextureHandle handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->RegisterInternal(texture, handle);
}

void TextureManager::UnRegister(const TextureHandle& handle) {
	TextureManager* manager = TextureManager::GetInstance();
	manager->UnRegisterInternal(handle);
}

void TextureManager::UnRegisterAll()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->textureMap.clear();
}