#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

typedef D3D12_DESCRIPTOR_RANGE DescriptorRange;
typedef D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc;

struct RootParamater {
    D3D12_ROOT_PARAMETER_TYPE ParameterType;
    std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorTable;
    D3D12_ROOT_CONSTANTS Constants;
    D3D12_ROOT_DESCRIPTOR Descriptor;
    D3D12_SHADER_VISIBILITY ShaderVisibility;
};

struct RootSignatureDesc {
    std::vector<RootParamater> RootParamaters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS Flags;
};

typedef std::vector<DescriptorRange> DescriptorRanges;
typedef std::vector<StaticSamplerDesc> StaticSamplerDescs;
typedef std::vector<RootParamater> RootParamaters;

class RootSignature
{
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> ptr = nullptr;
    RootSignatureDesc desc{};

	void Create();
};

