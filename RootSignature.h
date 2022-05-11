#pragma once
#include <d3d12.h>
#include <wrl.h>

class RootSignature
{
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> ptr = nullptr;
	D3D12_ROOT_SIGNATURE_DESC desc{};

	void Create();
};

