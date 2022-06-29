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

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		// ���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(T) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// �萔�o�b�t�@�̐���
		result = RDirectX::GetInstance()->device->CreateCommittedResource(
			&cbHeapProp, //�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc, //���\�[�X�ݒ�
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

	void Map() {
		HRESULT result;
		result = constBuff->Map(0, nullptr, (void**)&constMap); //�}�b�s���O
		assert(SUCCEEDED(result));
	}

	void UnMap() {
		constBuff->Unmap(0, nullptr);
	}
};

