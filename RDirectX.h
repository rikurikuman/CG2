#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

class RDirectX
{
public:
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQueue = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	void Init();
};

//RDirectX‚ğæ“¾‚·‚é
RDirectX* GetRDirectX();

//RDirectX‚ğ‰Šú‰»‚·‚é
void InitRDirectX();

//ƒtƒŒ[ƒ€I—¹–ˆ‚ÉŒÄ‚Ôˆ—
void EndFrame();