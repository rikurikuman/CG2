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
#include "Color.h"

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

	//現在のバックバッファ番号を取得する
	static UINT GetCurrentBackBufferIndex();

	//現在のバックバッファのリソースを取得する
	static ID3D12Resource* GetCurrentBackBufferResource();

	//現在のバックバッファのRTVハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle();

	//レンダーターゲットを指定する
	static void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle);

	//リソースバリアをPRESENT->RENDER_TARGETにする処理
	static void OpenResorceBarrier(ID3D12Resource* resource);

	//リソースバリアをRENDER_TARGET->PRESENTにする処理
	static void CloseResourceBarrier(ID3D12Resource* resource);

	//バックバッファを描画先にするための色んな処理をまとめて行う
	static void SetBackBufferToRenderTarget();

	/// <summary>
	/// レンダーターゲットを指定で塗りつぶす
	/// </summary>
	/// <param name="color">塗りつぶす色</param>
	static void ClearRenderTarget(Color color);

	//深度値をクリアする
	static void ClearDepthStencil();

	//コマンドリストを閉じて実行し、フリップして、コマンドリストを再び開ける
	static void RunDraw();

private:
	RDirectX() {};
	~RDirectX() = default;
	RDirectX(const RDirectX& a) {};
	RDirectX& operator=(const RDirectX&) { return *this; }

	D3D12_CPU_DESCRIPTOR_HANDLE nowRtvHandle = {};

	void InitInternal();
};