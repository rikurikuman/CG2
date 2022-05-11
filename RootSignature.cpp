#include "RootSignature.h"
#include "RDirectX.h"

void RootSignature::Create()
{
	HRESULT result;

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	assert(SUCCEEDED(result));

	result = GetRDirectX()->device->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&ptr));
	assert(SUCCEEDED(result));
}
