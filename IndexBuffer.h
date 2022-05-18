#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

class IndexBuffer
{
public:
	IndexBuffer() {};
	IndexBuffer(unsigned int* list, unsigned int size);
	IndexBuffer(std::vector<unsigned int> list);

	void Init(unsigned int* list, unsigned int size);
	void Init(std::vector<unsigned int> list);

	Microsoft::WRL::ComPtr<ID3D12Resource> buff = nullptr;
	D3D12_INDEX_BUFFER_VIEW view{};
};

