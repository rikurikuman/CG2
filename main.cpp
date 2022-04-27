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
	Texture* texA = TextureManager::Load("Resources/conflict.jpg");
	Texture* texB = TextureManager::Load("Resources/ougi.png");

	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;

	Sprite sprite(texB);
	Image3D image(texA, { 1, 1 });
	Cube cube(texA, { 1, 1 });
	cube.SetTexture(texB, Cube::Direction::Front);

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

		if (GetKey(DIK_1)) {
			materialBuff.constMap->color = XMFLOAT4(1, 0, 0, 0.5f);
		}
		else {
			materialBuff.constMap->color = XMFLOAT4(1, 1, 1, 1);
		}

		if (GetKey(DIK_SPACE)) {
			cube.transform.rotation.z += XMConvertToRadians(2.0f);
		}

		if (GetKey(DIK_UP)) {
			cube.transform.position.y += 0.1f;
		}
		if (GetKey(DIK_DOWN)) {
			cube.transform.position.y -= 0.1f;
		}
		if (GetKey(DIK_RIGHT)) {
			cube.transform.position.x += 0.1f;
		}
		if (GetKey(DIK_LEFT)) {
			cube.transform.position.x -= 0.1f;
		}

		cube.transform.UpdateMatrix();
		cube.UpdateFaces();

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

		cube.TransferBuffer(viewProjection);

		sprite.transform.position = { WIN_WIDTH / 2, WIN_HEIGHT / 2, 0 };
		sprite.transform.UpdateMatrix();

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

		GetRDirectX()->cmdList->SetPipelineState(GetRDirectX()->pipelineState.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(GetRDirectX()->rootSignature.Get());

		//プリミティブ形状設定
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRVヒープの設定コマンド
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		GetRDirectX()->cmdList->SetDescriptorHeaps(1, &_heap);

		//描画コマンド
		sprite.DrawCommands();
		cube.DrawCommands();

		//リソースバリアを表示に戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:描画から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:表示へ
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		EndFrame();
	}
	return 0;
}