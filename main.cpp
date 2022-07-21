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

	Model skydomeModel = Model::Load("Resources/skydome/", "skydome.obj");
	ModelObj skydome(&skydomeModel);

	Sprite controlDescText1(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), {0, 1});
	Sprite controlDescText2(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), {0, 1});
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();

	BillboardImage text1(TextDrawer::CreateStringTexture("必須課題:色が変わる三角形", "ＭＳ Ｐ明朝", 32), {0.25f, 0.25f});
	BillboardImage text2(TextDrawer::CreateStringTexture("αブレンドもしています", "ＭＳ Ｐ明朝", 32), {0.25f, 0.25f});
	text1.transform.position = { 0, 1.0f, 0 };
	text2.transform.position = { 0, 0.75f, 0 };
	text1.billboardY = true;
	text2.billboardY = true;

	TextureHandle texA = TextureManager::Load("Resources/conflict.jpg");
	TextureHandle texB = TextureManager::Load("Resources/A.png");
	TextureHandle texC = TextureManager::Load("Resources/B.png");
	TextureHandle texD = TextureManager::Load("Resources/X.png");
	TextureHandle texE = TextureManager::Load("Resources/Y.png");
	Cube cube(texA);
	cube.SetTexture(texB, Cube::Direction::Front);
	cube.SetTexture(texC, Cube::Direction::Left);
	cube.SetTexture(texD, Cube::Direction::Right);
	cube.SetTexture(texE, Cube::Direction::Back);
	cube.transform.position = { 0, 0, 10 };

	BillboardImage text3(TextDrawer::CreateStringTexture("複数の画像を読み込んで描画できます", "ＭＳ Ｐ明朝", 32), { 0.25f, 0.25f });
	text3.transform.position = { 0, 1, 10 };
	text3.billboardY = true;

	Model demoModel1 = Model::Load("Resources/", "encyu.obj");
	ModelObj demoModelObj1(&demoModel1);
	demoModelObj1.transform.position = { 0, 0, 20 };
	demoModelObj1.transform.scale = { 0.5f, 0.5f, 0.5f };
	demoModelObj1.transform.rotation.y = Util::AngleToRadian(180);
	demoModelObj1.transform.UpdateMatrix();

	Model demoModel2 = Model::Load("Resources/", "monkey.obj");
	ModelObj demoModelObj2(&demoModel2);
	demoModelObj2.transform.position = { 2, 0, 20 };
	demoModelObj2.transform.scale = { 0.5f, 0.5f, 0.5f };
	demoModelObj2.transform.rotation.y = Util::AngleToRadian(180);
	demoModelObj2.transform.UpdateMatrix();

	Model demoModel3 = Model::Load("Resources/", "hoge.obj");
	ModelObj demoModelObj3(&demoModel3);
	demoModelObj3.transform.position = { -2, 0, 20 };
	demoModelObj3.transform.scale = { 1, 1, 1 };
	demoModelObj3.transform.rotation.y = Util::AngleToRadian(180);
	demoModelObj3.transform.UpdateMatrix();

	BillboardImage text4(TextDrawer::CreateStringTexture("「.obj」形式のモデルを読み込んで描画できます", "ＭＳ Ｐ明朝", 32), { 0.25f, 0.25f });
	BillboardImage text5(TextDrawer::CreateStringTexture("周りに見えてる天球もモデルから読み込んでます", "ＭＳ Ｐ明朝", 32), { 0.25f, 0.25f });
	text4.transform.position = { 0, 1.25f, 20 };
	text4.billboardY = true;
	text5.transform.position = { 0, 1, 20 };
	text5.billboardY = true;

	Image3D text6(TextDrawer::CreateStringTexture("hogehogeあいうえintほげ", "ＭＳ Ｐ明朝", 128), { 3.0f, 3.0f });
	text6.transform.position = { 0, -50, 1.5f };
	text6.transform.rotation.x = Util::AngleToRadian(90);
	text6.transform.UpdateMatrix();
	 
	Image3D text7(TextDrawer::CreateStringTexture("テキストはttfからテクスチャを生成して貼っています", "ＭＳ Ｐ明朝", 128), { 3.0f, 3.0f });
	text7.transform.position = { 0, -50, -1.5f };
	text7.transform.rotation.x = Util::AngleToRadian(90);
	text7.transform.UpdateMatrix();

	DebugCamera camera({0, 0, -10});


	GraphicsPipeline simplePS = RDirectX::GetInstance()->pipelineState;
	simplePS.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //深度テストはするけど書き込まなくする
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

	HSVA triColor = { 0, 255, 255, 80 };

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
		
		cube.transform.rotation.y += XMConvertToRadians(1);
		cube.transform.UpdateMatrix();
		cube.UpdateFaces();

		camera.Update();
		text1.Update(camera.viewProjection);
		text2.Update(camera.viewProjection);
		text3.Update(camera.viewProjection);
		text4.Update(camera.viewProjection);
		text5.Update(camera.viewProjection);
		text6.TransferBuffer(camera.viewProjection);
		text7.TransferBuffer(camera.viewProjection);

		cube.TransferBuffer(camera.viewProjection);

		skydome.transform.scale = { 4,4,4 };
		skydome.transform.UpdateMatrix();
		skydome.TransferBuffer(camera.viewProjection);

		demoModelObj1.TransferBuffer(camera.viewProjection);
		demoModelObj2.TransferBuffer(camera.viewProjection);
		demoModelObj3.TransferBuffer(camera.viewProjection);

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

		cube.DrawCommands();
		skydome.DrawCommands();

		demoModelObj1.DrawCommands();
		demoModelObj2.DrawCommands();
		demoModelObj3.DrawCommands();

		RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());
		text1.DrawCommands();
		text2.DrawCommands();
		text3.DrawCommands();
		text4.DrawCommands();
		text5.DrawCommands();
		text6.DrawCommands();
		text7.DrawCommands();

		RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		controlDescText1.TransferBuffer();
		controlDescText2.TransferBuffer();
		controlDescText1.DrawCommands();
		controlDescText2.DrawCommands();

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

		//リソースバリアを表示に戻す
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();
	}
	return 0;
}