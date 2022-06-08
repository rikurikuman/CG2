#include "RootSignature.h"
#include "RDirectX.h"
#include <vector>

void RootSignature::Create()
{
	HRESULT result;

	D3D12_ROOT_SIGNATURE_DESC _desc{};

	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	for (RootParamater& param : desc.RootParamaters) {
		D3D12_ROOT_PARAMETER _param{};
		_param.ParameterType = param.ParameterType;
		switch (param.ParameterType) {
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
		{
			D3D12_ROOT_DESCRIPTOR_TABLE table{};
			if (param.DescriptorTable.size() > 0) {
				table.NumDescriptorRanges = param.DescriptorTable.size();
				table.pDescriptorRanges = &param.DescriptorTable[0];
			}
			else {
				table.NumDescriptorRanges = 0;
				table.pDescriptorRanges = nullptr;
			}
			_param.DescriptorTable = table;
			break;
		}	
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			_param.Constants = param.Constants;
			break;
		case D3D12_ROOT_PARAMETER_TYPE_CBV:
		case D3D12_ROOT_PARAMETER_TYPE_SRV:
		case D3D12_ROOT_PARAMETER_TYPE_UAV:
			_param.Descriptor = param.Descriptor;
			break;
		}
		_param.ShaderVisibility = param.ShaderVisibility;
		rootParams.emplace_back(_param);
	}

	if (rootParams.size() > 0) {
		_desc.pParameters = &rootParams[0];
		_desc.NumParameters = rootParams.size();
	}
	else {
		_desc.pParameters = nullptr;
		_desc.NumParameters = 0;
	}
	
	if (desc.StaticSamplers.size() > 0) {
		_desc.pStaticSamplers = &desc.StaticSamplers[0];
		_desc.NumStaticSamplers = desc.StaticSamplers.size();
	}
	else {
		_desc.pStaticSamplers = nullptr;
		_desc.NumStaticSamplers = 0;
	}
	
	_desc.Flags = desc.Flags;

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	assert(SUCCEEDED(result));

	result = GetRDirectX()->device->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&ptr));
	assert(SUCCEEDED(result));
}
