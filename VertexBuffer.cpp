#include "VertexBuffer.h"
#include "RDirectX.h"

VertexBuffer::VertexBuffer(Vertex* list, unsigned int size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(Vertex) * size);

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	Vertex* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU仮想アドレス
	view.SizeInBytes = dataSize; //頂点バッファのサイズ
	view.StrideInBytes = sizeof(Vertex); //頂点一個のサイズ
}

VertexBuffer::VertexBuffer(std::vector<Vertex> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(Vertex) * list.size());

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	Vertex* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU仮想アドレス
	view.SizeInBytes = dataSize; //頂点バッファのサイズ
	view.StrideInBytes = sizeof(Vertex); //頂点一個のサイズ
}
