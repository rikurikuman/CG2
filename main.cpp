#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifdef _DEBUG
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif

	//WindowsAPI
	InitRWindow();

	//DirectX
	InitRDirectX();

	//DirectInput
	InitInput();

	HRESULT result;

	//以下描画データ
	//頂点データ
	XMFLOAT3 vertices[] = {
		{ -0.5f, -0.5f, 0.0f }, //左下
		{ -0.5f, +0.5f, 0.0f }, //左上
		{ +0.5f, -0.5f, 0.0f }, //右下
	};
	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = GetRDirectX()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	vbView.SizeInBytes = sizeVB; //頂点バッファのサイズ
	vbView.StrideInBytes = sizeof(XMFLOAT3); //頂点一個のサイズ

	ComPtr<ID3DBlob> vsBlob = nullptr; //頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; //ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", //ファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0", //エントリ名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	result = D3DCompileFromFile(
		L"BasicPS.hlsl", //ファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0", //エントリ名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //標準

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.DepthClipEnable = true;

	// ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA全てのチャンネルを描画

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	pipelineDesc.SampleDesc.Count = 1; //1ピクセルにつき1回サンプリング

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = GetRDirectX()->device->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//セット
	pipelineDesc.pRootSignature = rootSignature.Get();

	// パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = GetRDirectX()->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	while (true) {
		GetRWindow()->ProcessMessage();

		if (GetRWindow()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		UpdateInput();

		//以下描画
		//バックバッファ番号の取得
		UINT bbIndex = GetRDirectX()->swapChain->GetCurrentBackBufferIndex();

		//リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = GetRDirectX()->backBuffers[bbIndex].Get();
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:表示から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:描画へ
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		//バックバッファを描画先にする
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRDirectX()->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * GetRDirectX()->device->GetDescriptorHandleIncrementSize(GetRDirectX()->rtvHeap->GetDesc().Type);
		GetRDirectX()->cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//画面クリア～
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; //青っぽい色でクリアする
		if (GetKey(DIK_SPACE)) {
			//赤っぽい色でクリアする
			clearColor[0] = 0.5f;
			clearColor[1] = 0.1f;
			clearColor[2] = 0.1f;
			clearColor[3] = 0.0f;
		}
		GetRDirectX()->cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//描画コマンド
		//ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = WIN_WIDTH;
		viewport.Height = WIN_HEIGHT;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		GetRDirectX()->cmdList->RSSetViewports(1, &viewport);

		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + WIN_WIDTH;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + WIN_HEIGHT;
		GetRDirectX()->cmdList->RSSetScissorRects(1, &scissorRect);

		GetRDirectX()->cmdList->SetPipelineState(pipelineState.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(rootSignature.Get());

		//プリミティブ形状設定
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファビューの設定コマンド
		GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vbView);

		//描画コマンド
		GetRDirectX()->cmdList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画

		//リソースバリアを表示に戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:描画から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:表示へ
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		EndFrame();
	}
	return 0;
}