#include "GraphicsPipeline.h"
#include "RDirectX.h"

void GraphicsPipeline::Create()
{
	HRESULT result;
	result = GetRDirectX()->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&ptr));
	assert(SUCCEEDED(result));
}
