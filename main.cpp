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
#include <DirectXTex.h>
#include "Transform.h"
#include "Texture.h"
#include "Sprite.h"
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "Image3D.h"
#include "Cube.h"
#include "Matrix4.h"
#include "Util.h"
#include "Model.h"
#include "ModelObj.h"

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

	//いろいろ

	Model model = Model::Load("Resources/hoge.obj");
	Material hogeMate;
	hogeMate.color = { 1, 0, 0, 1 };
	ModelObj hogeObj(&model);
	hogeObj.material = hogeMate;

	Texture* texA = TextureManager::Load("Resources/conflict.jpg");
	Texture* texB = TextureManager::Load("Resources/bg.png");

	RConstBuffer<MaterialBuffer> materialBuff;

	Sprite sprite(texB);
	Image3D image(texA, { 1, 1 });
	Cube cubeA(texB, { 1.768f, 1 });
	Cube cubeB(texA, { 1, 1 });
	cubeB.transform.position = { 0, 0, 10 };

	Transform transform;

	ViewProjection viewProjection;
	viewProjection.eye = { 0, 0, -10 };
	viewProjection.aspect = (float)WIN_WIDTH / WIN_HEIGHT;
	viewProjection.UpdateMatrix();

	float angle = 0.0f; //カメラの回転角

	while (true) {
		GetRWindow()->ProcessMessage();

		if (GetRWindow()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		UpdateInput();

		if (GetKeyDown(DIK_F1)) {
			Util::debugBool = !Util::debugBool;
		}

		if (GetKey(DIK_1)) {
			materialBuff.constMap->color = { 1, 0, 0, 0.5f };
		}
		else {
			materialBuff.constMap->color = { 1, 1, 1, 1 };
		}

		if (GetKey(DIK_SPACE)) {
			cubeA.transform.rotation.z += XMConvertToRadians(10.0f);
		}

		if (GetKey(DIK_UP)) {
			cubeA.transform.position.y += 0.1f;
		}
		if (GetKey(DIK_DOWN)) {
			cubeA.transform.position.y -= 0.1f;
		}
		if (GetKey(DIK_RIGHT)) {
			cubeA.transform.position.x += 0.1f;
		}
		if (GetKey(DIK_LEFT)) {
			cubeA.transform.position.x -= 0.1f;
		}

		cubeA.transform.UpdateMatrix();
		cubeA.UpdateFaces();
		
		cubeB.transform.rotation.y += XMConvertToRadians(5);
		cubeB.transform.UpdateMatrix();
		cubeB.UpdateFaces();

		if (GetKey(DIK_W)) {
			viewProjection.eye.z += 0.1f;
		}
		if (GetKey(DIK_S)) {
			viewProjection.eye.z -= 0.1f;
		}
		if (GetKey(DIK_A)) {
			viewProjection.eye.x -= 0.1f;
		}
		if (GetKey(DIK_D)) {
			viewProjection.eye.x += 0.1f;
		}
		viewProjection.UpdateMatrix();

		image.TransferBuffer(viewProjection);
		cubeA.TransferBuffer(viewProjection);
		cubeB.TransferBuffer(viewProjection);

		sprite.transform.position = { WIN_WIDTH / 2, WIN_HEIGHT / 2, 0 };
		sprite.transform.scale = { 0.5f, 0.5f, 1 };
		sprite.transform.UpdateMatrix();

		hogeObj.TransferBuffer(viewProjection);

		//以下描画
		//バックバッファ番号の取得
		UINT bbIndex = GetRDirectX()->swapChain->GetCurrentBackBufferIndex();

		//リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = GetRDirectX()->backBuffers[bbIndex].Get();
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:表示から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:描画へ
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		//バックバッファを描画先にする(レンダーターゲットの設定)
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRDirectX()->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * GetRDirectX()->device->GetDescriptorHandleIncrementSize(GetRDirectX()->rtvHeap->GetDesc().Type);

		//深度ステンシルも設定
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetRDirectX()->dsvHeap->GetCPUDescriptorHandleForHeapStart();

		//セット
		GetRDirectX()->cmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

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
		GetRDirectX()->cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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

		GetRDirectX()->cmdList->SetPipelineState(GetRDirectX()->pipelineState.ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(GetRDirectX()->rootSignature.ptr.Get());

		//プリミティブ形状設定
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRVヒープの設定コマンド
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		GetRDirectX()->cmdList->SetDescriptorHeaps(1, &_heap);

		//描画コマンド

		GetRDirectX()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		//sprite.DrawCommands();

		GetRDirectX()->cmdList->SetPipelineState(GetRDirectX()->pipelineState.ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(GetRDirectX()->rootSignature.ptr.Get());

		//cubeA.DrawCommands();
		//cubeB.DrawCommands();
		hogeObj.DrawCommands();

		//リソースバリアを表示に戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:描画から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:表示へ
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		EndFrame();
	}
	return 0;
}