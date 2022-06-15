#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Shader.h"

typedef std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;

struct PipelineStateDesc {
    ID3D12RootSignature* pRootSignature;
    Shader VS;
    Shader PS;
    Shader DS;
    Shader HS;
    Shader GS;
    D3D12_STREAM_OUTPUT_DESC StreamOutput;
    D3D12_BLEND_DESC BlendState;
    UINT SampleMask;
    D3D12_RASTERIZER_DESC RasterizerState;
    D3D12_DEPTH_STENCIL_DESC DepthStencilState;
    InputLayout InputLayout;
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
    UINT NumRenderTargets;
    DXGI_FORMAT RTVFormats[8];
    DXGI_FORMAT DSVFormat;
    DXGI_SAMPLE_DESC SampleDesc;
    UINT NodeMask;
    D3D12_CACHED_PIPELINE_STATE CachedPSO;
    D3D12_PIPELINE_STATE_FLAGS Flags;
};

class GraphicsPipeline
{
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> ptr = nullptr;
    PipelineStateDesc desc{};

	void Create();
};

