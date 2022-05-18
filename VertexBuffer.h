#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Vertex.h"

class VertexBuffer
{
public:
	VertexBuffer() {};
	VertexBuffer(Vertex* list, unsigned int size);
	VertexBuffer(std::vector<Vertex> list);

	Microsoft::WRL::ComPtr<ID3D12Resource> buff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW view{};
};

