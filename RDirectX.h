#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include "Texture.h"
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "Shader.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;
	Shader basicVS;
	Shader basicPS;
	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	//DirectX初期化
	static void Init();
	static RDirectX* GetInstance();

	//コマンドリストを閉じて実行し、フリップして、コマンドリストを再び開ける
	static void RunDraw();

private:
	RDirectX() {};
	~RDirectX() = default;
	RDirectX(const RDirectX& a) {};
	RDirectX& operator=(const RDirectX&) { return *this; }

	void InitInternal();
};