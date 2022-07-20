#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include <d3dcompiler.h>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"
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
#include "DebugCamera.h"
#include "BillboardImage.h"
#include "TextDrawer.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

void DrawSimpleTriangle(Color color, ViewProjection viewProjection);

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//WindowsAPI
	RWindow::Init();

	//DirectX
	RDirectX::Init();

	//DirectInput
	RInput::Init();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(RDirectX::GetInstance()->device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif

	//いろいろ

	///////////////////

	Model model = Model::Load("Resources/skydome/", "skydome.obj");
	ModelObj hogeObj(&model);

	TextureHandle texA = TextureManager::Load("Resources/conflict.jpg");
	TextureHandle texB = TextureManager::Load("Resources/bg.png");

	Sprite sprite(texB, { 0, 0 });

	Sprite controlDescText1(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), {0, 1});
	Sprite controlDescText2(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), {0, 1});
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();

	BillboardImage textA(TextDrawer::CreateStringTexture("必須課題:色が変わる三角形", "ＭＳ Ｐ明朝", 32), {0.25f, 0.25f});
	BillboardImage textB(TextDrawer::CreateStringTexture("元のテクスチャに緑成分がないので緑は出ません", "ＭＳ Ｐ明朝", 32), {0.25f, 0.25f});
	textA.transform.position = { 0, 1.0f, 0 };
	textB.transform.position = { 0, 0.75f, 0 };
	textA.billboardY = true;
	textB.billboardY = true;

	BillboardImage text3(TextDrawer::CreateStringTexture("箱とかの描画もできます", "ＭＳ Ｐ明朝", 32), {0.25f, 0.25f});
	text3.transform.position = { 0, 1, 10 };
	text3.billboardY = true;

	Cube cubeA(texB);
	Cube cubeB(texA);
	cubeA.transform.position = { 0, -10, 0 };
	cubeB.transform.position = { 0, 0, 10 };

	DebugCamera camera({0, 0, -10});

	/// 課題用

	GraphicsPipeline simplePS = RDirectX::GetInstance()->pipelineState;
	simplePS.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //カリングしなくする
	simplePS.Create();

	VertexPNU simpleTriVertices[] = {
		{{ -0.5f, -0.5, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f,  0.5, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{  0.5f, -0.5, 0.0f }, {}, {1.0f, 1.0f}}, //右下
	};

	//頂点インデックスデータ
	UINT simpleTriIndices[] = {
		0, 1, 2,
	};

	VertexPNU::CalcNormalVec(simpleTriVertices, simpleTriIndices, _countof(simpleTriIndices));

	VertexBuffer simpleTriVertBuff(simpleTriVertices, _countof(simpleTriVertices));
	IndexBuffer simpleTriIndexBuff(simpleTriIndices, _countof(simpleTriIndices));

	Material simpleTriMaterial;
	simpleTriMaterial.color = Color(1, 1, 1, 1);
	RConstBuffer<MaterialBuffer> simpleTriMaterialBuff;
	simpleTriMaterial.Transfer(simpleTriMaterialBuff.constMap);

	RConstBuffer<TransformBuffer> simpleTriTransformBuff;
	Transform simpleTriTransform;
	simpleTriTransform.Transfer(simpleTriTransformBuff.constMap);

	RConstBuffer<ViewProjectionBuffer> simpleTriViewProjectionBuff;

	HSVA triColor = { 0, 255, 255, 255 };

	//////////////////////////////////////

	while (true) {
		RWindow::ProcessMessage();

		if (RWindow::GetInstance()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		if (GetActiveWindow() == RWindow::GetWindowHandle()) {
			RWindow::SetMouseHideFlag(true);
			RWindow::SetMousePos(RWindow::GetWidth() / 2, RWindow::GetHeight() / 2);
		}
		else {
			RWindow::SetMouseHideFlag(false);
		}

		RInput::Update();

		if (RInput::GetKeyDown(DIK_F1)) {
			Util::debugBool = !Util::debugBool;
		}

		if (RInput::GetKey(DIK_SPACE)) {
			cubeA.transform.rotation.z += XMConvertToRadians(10.0f);
		}

		if (RInput::GetKey(DIK_UP)) {
			cubeA.transform.position.z += 0.1f;
		}
		if (RInput::GetKey(DIK_DOWN)) {
			cubeA.transform.position.z -= 0.1f;
		}
		if (RInput::GetKey(DIK_RIGHT)) {
			cubeA.transform.position.x += 0.1f;
		}
		if (RInput::GetKey(DIK_LEFT)) {
			cubeA.transform.position.x -= 0.1f;
		}

		if (RInput::GetKey(DIK_NUMPAD8)) {
			cubeA.transform.scale.y += 0.1f;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			cubeA.transform.scale.y -= 0.1f;
		}
		if (RInput::GetKey(DIK_NUMPAD6)) {
			cubeA.transform.scale.x += 0.1f;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			cubeA.transform.scale.x -= 0.1f;
		}

		cubeA.transform.UpdateMatrix();
		cubeA.UpdateFaces();
		
		cubeB.transform.rotation.y += XMConvertToRadians(1);
		cubeB.transform.UpdateMatrix();
		cubeB.UpdateFaces();

		camera.Update();
		textA.Update(camera.viewProjection);
		textB.Update(camera.viewProjection);
		text3.Update(camera.viewProjection);

		cubeA.TransferBuffer(camera.viewProjection);
		cubeB.TransferBuffer(camera.viewProjection);

		sprite.transform.position = { 0, 0, 0 };
		sprite.transform.scale = { 0.1f, 0.1f, 1 };
		sprite.transform.UpdateMatrix();
		sprite.TransferBuffer();

		hogeObj.transform.scale = { 4,4,4 };
		hogeObj.transform.UpdateMatrix();
		hogeObj.TransferBuffer(camera.viewProjection);

		triColor.h++;
		simpleTriMaterialBuff.constMap->color = Color::convertFromHSVA(triColor);
		simpleTriViewProjectionBuff.constMap->matrix = camera.viewProjection.matrix;

		//以下描画
		RDirectX::SetBackBufferToRenderTarget();

		//画面クリア～
		if (RInput::GetKey(DIK_SPACE)) {
			RDirectX::ClearRenderTarget(Color(0.5f, 0.1f, 0.1f, 0.0f)); //赤っぽい色でクリアする
		}
		else {
			RDirectX::ClearRenderTarget(Color(0.1f, 0.25f, 0.5f, 0.0f)); //青っぽい色でクリアする
		}
		
		//深度値もクリア
		RDirectX::ClearDepthStencil();

		//描画コマンド
		//ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = WIN_WIDTH;
		viewport.Height = WIN_HEIGHT;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		RDirectX::GetInstance()->cmdList->RSSetViewports(1, &viewport);

		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + WIN_WIDTH;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + WIN_HEIGHT;
		RDirectX::GetInstance()->cmdList->RSSetScissorRects(1, &scissorRect);

		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

		//プリミティブ形状設定
		RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRVヒープの設定コマンド
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		RDirectX::GetInstance()->cmdList->SetDescriptorHeaps(1, &_heap);

		//描画コマンド

		//色変わる三角形
		RDirectX::GetInstance()->cmdList->SetPipelineState(simplePS.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &simpleTriVertBuff.view);
		RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&simpleTriIndexBuff.view);
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, simpleTriMaterialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, simpleTriTransformBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, simpleTriViewProjectionBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get("").gpuHandle);
		RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(3, 1, 0, 0, 0); // 全ての頂点を使って描画

		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

		cubeA.DrawCommands();
		cubeB.DrawCommands();
		hogeObj.DrawCommands();

		textA.DrawCommands();
		textB.DrawCommands();
		text3.DrawCommands();

		RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		controlDescText1.TransferBuffer();
		controlDescText2.TransferBuffer();
		controlDescText1.DrawCommands();
		controlDescText2.DrawCommands();

		//sprite.DrawCommands();

		//リソースバリアを表示に戻す
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();
	}
	return 0;
}

void DrawSimpleTriangle(Color color, ViewProjection viewProjection)
{
	
}
