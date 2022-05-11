#pragma once
#include <d3d12.h>
#include <wrl.h>

class GraphicsPipeline
{
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> ptr = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

	void Create();
};

