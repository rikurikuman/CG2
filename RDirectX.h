#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include "Texture.h"
#include "RootSignature.h"
#include "GraphicsPipeline.h"

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
	ComPtr<ID3DBlob> basicVSBlob = nullptr; //頂点シェーダオブジェクト
	ComPtr<ID3DBlob> basicPSBlob = nullptr; //ピクセルシェーダオブジェクト
	//ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	void Init();
};

//RDirectXを取得する
RDirectX* GetRDirectX();

//RDirectXを初期化する
void InitRDirectX();

//フレーム終了毎に呼ぶ処理
void EndFrame();