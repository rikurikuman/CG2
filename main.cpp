#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
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
#include "SphereCollision.h"
#include "Raycast.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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

	Sprite controlDescText1(TextDrawer::CreateStringTexture("ESC:終了", "", 24), { 0, 1 });
	Sprite controlDescText2(TextDrawer::CreateStringTexture("WASD:移動, マウスで視点操作", "", 24), {0, 1});
	Sprite controlDescText3(TextDrawer::CreateStringTexture("Space:上昇, LShift:下降", "", 24), {0, 1});
	controlDescText1.transform.position = { 0, (float)RWindow::GetHeight() - 48, 0 };
	controlDescText2.transform.position = { 0, (float)RWindow::GetHeight() - 24, 0 };
	controlDescText3.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDescText1.transform.UpdateMatrix();
	controlDescText2.transform.UpdateMatrix();
	controlDescText3.transform.UpdateMatrix();

	TextureHandle tex = TextureManager::Load("Resources/conflict.jpg");
	Cube cubeA(tex);
	Cube cubeB(tex);
	cubeA.transform.position = { 0, 0, 10 };
	SphereCollision cubeColA(cubeA.transform.position, 1);
	Raycast cubeColB(cubeB.transform.position, Vector3(0, 0, 1), 10);

	Image3D text(TextDrawer::CreateStringTexture("hogehogeあいうえintほげ", "ＭＳ Ｐ明朝", 128), { 3.0f, 3.0f });
	text.transform.position = { 0, 0, 20 };
	text.transform.UpdateMatrix();

	DebugCamera camera({0, 0, -10});

	//グリッド
	RootSignature gridRoot = RDirectX::GetInstance()->rootSignature;

	RootParamaters gridRootParams(2);
	//定数バッファ0番(Material)
	gridRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	gridRootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	gridRootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	gridRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(ViewProjection)
	gridRootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	gridRootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	gridRootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	gridRootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	gridRoot.desc.RootParamaters = gridRootParams;
	gridRoot.Create();

	GraphicsPipeline gridPS = RDirectX::GetInstance()->pipelineState;
	gridPS.desc.VS = Shader("Shader/GridVS.hlsl", "main", "vs_5_0");
	gridPS.desc.PS = Shader("Shader/GridPS.hlsl", "main", "ps_5_0");

	//gridPS.desc.RasterizerState.DepthClipEnable = false;
	//gridPS.desc.DepthStencilState.DepthEnable = false;
	//gridPS.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	InputLayout gridInputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};
	gridPS.desc.InputLayout = gridInputLayout;
	gridPS.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	gridPS.desc.pRootSignature = gridRoot.ptr.Get();
	gridPS.Create();

	vector<VertexP> gridVertices;

	for (int i = -100; i <= 100; i++) {
		gridVertices.push_back(Vector3{ (float)i, -10, 100 });
		gridVertices.push_back(Vector3{ (float)i, -10, -100 });
	}

	for (int i = -100; i <= 100; i++) {
		gridVertices.push_back(Vector3{ 100, -10, (float)i });
		gridVertices.push_back(Vector3{ -100, -10, (float)i });
	}

	VertexBuffer gridVertBuff(gridVertices);

	Material gridMaterial;
	gridMaterial.color = Color(0, 1, 1, 0.25f);
	RConstBuffer<MaterialBuffer> gridMaterialBuff;
	gridMaterial.Transfer(gridMaterialBuff.constMap);

	RConstBuffer<ViewProjectionBuffer> gridViewProjectionBuff;

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

		float moveSpeed = 0.1f;
		Vector3 moveVec;

		if (RInput::GetKey(DIK_NUMPAD8)) {
			moveVec.z += moveSpeed;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			moveVec.z -= moveSpeed;
		}
		if (RInput::GetKey(DIK_NUMPAD6)) {
			moveVec.x += moveSpeed;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			moveVec.x -= moveSpeed;
		}

		if (RInput::GetKeyDown(DIK_NUMPAD5)) {
			if (cubeColB.distance < 0) {
				cubeColB.distance = 10;
			}
			else {
				cubeColB.distance = -1;
			}
		}

		cubeB.transform.position += moveVec;
		cubeB.transform.UpdateMatrix();
		cubeB.UpdateFaces();
		
		cubeA.transform.rotation.y += XMConvertToRadians(1);
		cubeA.transform.UpdateMatrix();
		cubeA.UpdateFaces();

		cubeColB.pos = cubeB.transform.position;

		camera.Update();
		text.TransferBuffer(camera.viewProjection);

		cubeA.TransferBuffer(camera.viewProjection);
		cubeB.TransferBuffer(camera.viewProjection);

		skydome.transform.scale = { 4,4,4 };
		skydome.transform.UpdateMatrix();
		skydome.TransferBuffer(camera.viewProjection);

		gridViewProjectionBuff.constMap->matrix = camera.viewProjection.matrix;

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

		cubeA.DrawCommands();
		cubeB.DrawCommands();
		skydome.DrawCommands();

		if (cubeColA.Collide(cubeColB)) {
			RDirectX::GetInstance()->cmdList->SetPipelineState(TextDrawer::GetInstance()->pipeline.ptr.Get());
			text.DrawCommands();
		}

		RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		controlDescText1.TransferBuffer();
		controlDescText2.TransferBuffer();
		controlDescText3.TransferBuffer();
		controlDescText1.DrawCommands();
		controlDescText2.DrawCommands();
		controlDescText3.DrawCommands();

		//グリッド
		RDirectX::GetInstance()->cmdList->SetPipelineState(gridPS.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(gridRoot.ptr.Get());
		RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &gridVertBuff.view);
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(0, gridMaterialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, gridViewProjectionBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->DrawInstanced((UINT)gridVertices.size(), 1, 0, 0);

		vector<VertexP> rayVertices;
		rayVertices.push_back(cubeColB.pos);
		if (cubeColB.distance < 0) {
			rayVertices.push_back(cubeColB.pos + cubeColB.direction * 10000);
		}
		else {
			rayVertices.push_back(cubeColB.pos + cubeColB.direction * cubeColB.distance);
		}
		VertexBuffer rayVertBuff(rayVertices);

		Material rayMaterial;
		rayMaterial.color = Color(1, 1, 1, 1);
		RConstBuffer<MaterialBuffer> rayMaterialBuff;
		rayMaterial.Transfer(rayMaterialBuff.constMap);

		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &rayVertBuff.view);
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(0, rayMaterialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, gridViewProjectionBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->DrawInstanced((UINT)rayVertices.size(), 1, 0, 0);

		//リソースバリアを表示に戻す
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}
	return 0;
}