#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "RDirectX.h"

template <typename T>
class RConstBuffer
{
private:
	void Init() {
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(T) + 0xff) & ~0xff; //256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// 定数バッファの生成
		result = GetRDirectX()->device->CreateCommittedResource(
			&cbHeapProp, //ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc, //リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff)
		);
		assert(SUCCEEDED(result));
	}

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;
	T* constMap = nullptr;


	RConstBuffer() {
		Init();
		Map();
	}
	~RConstBuffer() {
		UnMap();
	}

	inline void Map() {
		HRESULT result;
		result = constBuff->Map(0, nullptr, (void**)&constMap); //マッピング
		assert(SUCCEEDED(result));
	}

	inline void UnMap() {
		constBuff->Unmap(0, nullptr);
	}
};

