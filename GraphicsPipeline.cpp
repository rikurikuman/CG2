#include "GraphicsPipeline.h"
#include "RDirectX.h"

void GraphicsPipeline::Create()
{
	HRESULT result;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc{};

    _desc.pRootSignature = desc.pRootSignature;

    if (desc.VS.succeeded) {
        _desc.VS.pShaderBytecode = desc.VS.shaderBlob->GetBufferPointer();
        _desc.VS.BytecodeLength  = desc.VS.shaderBlob->GetBufferSize();
    }

    if (desc.PS.succeeded) {
        _desc.PS.pShaderBytecode = desc.PS.shaderBlob->GetBufferPointer();
        _desc.PS.BytecodeLength  = desc.PS.shaderBlob->GetBufferSize();
    }

    if (desc.DS.succeeded) {
        _desc.DS.pShaderBytecode = desc.DS.shaderBlob->GetBufferPointer();
        _desc.DS.BytecodeLength  = desc.DS.shaderBlob->GetBufferSize();
    }

    if (desc.HS.succeeded) {
        _desc.HS.pShaderBytecode = desc.HS.shaderBlob->GetBufferPointer();
        _desc.HS.BytecodeLength  = desc.HS.shaderBlob->GetBufferSize();
    }

    if (desc.GS.succeeded) {
        _desc.GS.pShaderBytecode = desc.GS.shaderBlob->GetBufferPointer();
        _desc.GS.BytecodeLength  = desc.GS.shaderBlob->GetBufferSize();
    }

    _desc.StreamOutput = desc.StreamOutput;
    _desc.BlendState = desc.BlendState;
    _desc.SampleMask = desc.SampleMask;
    _desc.RasterizerState = desc.RasterizerState;
    _desc.DepthStencilState = desc.DepthStencilState;

    if (desc.InputLayout.size() > 0) {
        _desc.InputLayout.pInputElementDescs = &desc.InputLayout[0];
        _desc.InputLayout.NumElements = desc.InputLayout.size();
    }
    else {
        _desc.InputLayout.pInputElementDescs = nullptr;
        _desc.InputLayout.NumElements = 0;
    }

    _desc.IBStripCutValue = desc.IBStripCutValue;
    _desc.PrimitiveTopologyType = desc.PrimitiveTopologyType;
    _desc.NumRenderTargets = desc.NumRenderTargets;

    for (int i = 0; i < 8; i++) {
        _desc.RTVFormats[i] = desc.RTVFormats[i];
    }
    
    _desc.DSVFormat = desc.DSVFormat;
    _desc.SampleDesc = desc.SampleDesc;
    _desc.NodeMask = desc.NodeMask;
    _desc.CachedPSO = desc.CachedPSO;
    _desc.Flags = desc.Flags;

	result = RDirectX::GetInstance()->device->CreateGraphicsPipelineState(&_desc, IID_PPV_ARGS(&ptr));
	assert(SUCCEEDED(result));
}
