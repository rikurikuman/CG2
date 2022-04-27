#include "Image3D.h"
#include "RDirectX.h"
#include "Vertex.h"

Image3D::Image3D(Texture* texture, XMFLOAT2 size)
{
	this->texture = texture;
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}
	
void Image3D::Init()
{
	HRESULT result;

	//以下描画データ

	//頂点データ
	Vertex vertices[] = {
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f * size.x,  0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{  0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{  0.5f * size.x,  0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	uint16_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
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
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//インデックスバッファリソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
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
		IID_PPV_ARGS(&indexBuff)
	);

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	vbView.SizeInBytes = sizeVB; //頂点バッファのサイズ
	vbView.StrideInBytes = sizeof(vertices[0]); //頂点一個のサイズ

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Image3D::TransferBuffer(ViewProjection viewprojection)
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap, viewprojection.matrix);
}

void Image3D::DrawCommands()
{
	//頂点バッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	//インデックスバッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetIndexBuffer(&ibView);

	//定数バッファビューの設定コマンド
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, texture->gpuHandle);

	//描画コマンド
	GetRDirectX()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}
