#include "RDirectX.h"
#include "RWindow.h"
#include <vector>
#include <memory>

using namespace std;

unique_ptr<RDirectX> directX;

void InitRDirectX() {
	directX = unique_ptr<RDirectX>(new RDirectX());
	directX->Init();
}

RDirectX* GetRDirectX() {
	return directX.get();
}

void RDirectX::Init() {
	HRESULT result;

	//DXGIファクトリー生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	vector<ComPtr<IDXGIAdapter4>> adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高い順に全てのアダプターを列挙する
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプタの選別
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;

		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを除外して、
		//最初に見つかったアダプタ（パフォーマンスが最も高い）を採用する
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			tmpAdapter = adapters[i];
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK) {
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));
	assert(SUCCEEDED(result));

	//コマンドキュー
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(SUCCEEDED(result));

	//スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色情報の書式
	swapChainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //バックバッファ用
	swapChainDesc.BufferCount = 2; //バッファ数を2に
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//生成
	ComPtr<IDXGISwapChain1> _swapChain1;
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(), GetRWindow()->GetWindowHandle(), &swapChainDesc, nullptr, nullptr,
		&_swapChain1);
	_swapChain1.As(&swapChain);
	assert(SUCCEEDED(result));

	//デスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; //裏表の二つ
	//生成
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	assert(SUCCEEDED(result));

	//バックバッファ
	backBuffers.resize(swapChainDesc.BufferCount);

	//スワップチェーンの全てのバッファに対して処理
	for (size_t i = 0; i < backBuffers.size(); i++) {
		//スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//裏か表かでアドレスをずらす
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}

	//フェンスの生成
	UINT64 fenceVal = 0;

	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

void EndFrame() {
	HRESULT result;

	//命令のクローズ
	result = directX->cmdList->Close();
	assert(SUCCEEDED(result));
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { directX->cmdList.Get() };
	directX->cmdQueue->ExecuteCommandLists(1, cmdLists);

	//フリップ
	result = directX->swapChain->Present(1, 0);
	assert(SUCCEEDED(result));

	directX->cmdQueue->Signal(directX->fence.Get(), ++directX->fenceVal);
	if (directX->fence->GetCompletedValue() != directX->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		directX->fence->SetEventOnCompletion(directX->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//キューをクリア
	result = directX->cmdAllocator->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = directX->cmdList->Reset(directX->cmdAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));
}