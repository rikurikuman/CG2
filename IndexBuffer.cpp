#include "IndexBuffer.h"
#include "RDirectX.h"

IndexBuffer::IndexBuffer(unsigned int* list, unsigned int size)
{
	Init(list, size);
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> list)
{
	Init(list);
}

void IndexBuffer::Init(unsigned int* list, unsigned int size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * size);

	//インデックスバッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//インデックスバッファ生成
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU上のバッファに対応した仮想メモリを取得
	//インデックスバッファをマッピング
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (UINT i = 0; i < size; i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}

void IndexBuffer::Init(std::vector<unsigned int> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * list.size());

	//インデックスバッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//インデックスバッファ生成
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU上のバッファに対応した仮想メモリを取得
	//インデックスバッファをマッピング
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (UINT i = 0; i < list.size(); i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}
