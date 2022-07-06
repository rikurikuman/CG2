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

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
#ifdef _DEBUG
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif
	//WindowsAPI
	RWindow::Init();

	//DirectX
	RDirectX::Init();

	//DirectInput
	RInput::Init();

	//いろいろ

	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };

	HFONT _font = CreateFont(128, 0, 0, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"ＭＳ Ｐ明朝");

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, _font);
	wchar_t* c = L"あ";
	UINT code = (UINT)*c;

	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// オブジェクトの開放
	SelectObject(hdc, oldFont);
	DeleteObject(_font);
	ReleaseDC(NULL, hdc);

	int fontWidth = (GM.gmBlackBoxX + 3) / 4 * 4;
	int fontHeight = GM.gmBlackBoxY;
	int fontDataCount = fontWidth * fontHeight;

	OutputDebugString((wstring(L"fontWidth: ") + to_wstring(fontWidth) + L"(" + to_wstring(GM.gmBlackBoxX) + L")\n").c_str());
	OutputDebugString((wstring(L"fontHeight: ") + to_wstring(fontHeight) + L"\n").c_str());

	Color* imageData = new Color[fontDataCount];

	for (size_t i = 0; i < fontDataCount; i++) {
		float grayScale = (float)ptr[i];
		imageData[i].r = 1.0f;
		imageData[i].g = 1.0f;
		imageData[i].b = 1.0f;
		imageData[i].a = (grayScale * 255.0f / 16) / 255.0f;
	}

	Texture texture = Texture();

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = fontWidth;
	textureResourceDesc.Height = fontHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	RDirectX::GetInstance()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);

	texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * fontWidth,
		sizeof(Color) * fontDataCount
	);

	delete[] imageData;

	TextureManager::Register(texture, "テスト");

	Sprite testSprite("テスト", {0.0f, 0.0f});
	Image3D testFontImage("テスト", { 1.0f, 1.0f });

	//いろいろ(GSてすと)

	Shader::Register("TestVS", Shader("Shader/TestVS.hlsl", "main", "vs_5_0"));
	Shader::Register("TestGS", Shader("Shader/TestGS.hlsl", "main", "gs_5_0"));
	Shader::Register("TestPS", Shader("Shader/TestPS.hlsl", "main", "ps_5_0"));

	RootSignature testRS = RDirectX::GetInstance()->rootSignature;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(2);
	//テクスチャレジスタ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	testRS.desc.RootParamaters = rootParams;
	testRS.Create();

	GraphicsPipeline testPL = RDirectX::GetInstance()->pipelineState;
	testPL.desc.VS = Shader("Shader/TestVS.hlsl", "main", "vs_5_0");
	testPL.desc.GS = Shader("Shader/TestGS.hlsl", "main", "gs_5_0");
	testPL.desc.PS = Shader("Shader/TestPS.hlsl", "main", "ps_5_0");
	testPL.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	testPL.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};
	testPL.desc.pRootSignature = testRS.ptr.Get();
	testPL.Create();

	VertexBuffer testPoint(std::vector<VertexP> { VertexP() });
	RConstBuffer<ViewProjectionBuffer> testViewProBuff;

	///////////////////

	Model model = Model::Load("Resources/skydome/", "skydome.obj");
	ModelObj hogeObj(&model);

	TextureHandle texA = TextureManager::Load("Resources/conflict.jpg");
	TextureHandle texB = TextureManager::Load("Resources/bg.png");

	RConstBuffer<MaterialBuffer> materialBuff;

	Sprite sprite(texB, {0, 0});
	Image3D image(texA, { 1, 1 });
	Cube cubeA(texB, {1.768f, 1});
	Cube cubeB(texA, { 1, 1 });
	cubeA.transform.position = { 0, -10, 0 };
	cubeB.transform.position = { 0, 0, 10 };

	ViewProjection viewProjection;
	viewProjection.eye = { 0, 0, -10 };
	viewProjection.aspect = (float)WIN_WIDTH / WIN_HEIGHT;
	viewProjection.UpdateMatrix();

	float angle = 0.0f; //カメラの回転角

	DebugCamera camera({0, 0, -10});

	while (true) {
		RWindow::ProcessMessage();

		if (RWindow::GetInstance()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		RInput::Update();

		if (RInput::GetKeyDown(DIK_F1)) {
			Util::debugBool = !Util::debugBool;
		}

		if (RInput::GetKey(DIK_1)) {
			materialBuff.constMap->color = { 1, 0, 0, 0.5f };
		}
		else {
			materialBuff.constMap->color = { 1, 1, 1, 1 };
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
		
		cubeB.transform.rotation.y += XMConvertToRadians(5);
		cubeB.transform.UpdateMatrix();
		cubeB.UpdateFaces();

		if (RInput::GetKey(DIK_W)) {
			viewProjection.eye.z += 0.1f;
		}
		if (RInput::GetKey(DIK_S)) {
			viewProjection.eye.z -= 0.1f;
		}
		if (RInput::GetKey(DIK_A)) {
			viewProjection.eye.x -= 0.1f;
		}
		if (RInput::GetKey(DIK_D)) {
			viewProjection.eye.x += 0.1f;
		}
		viewProjection.UpdateMatrix();

		camera.Update();

		image.TransferBuffer(camera.viewProjection);
		cubeA.TransferBuffer(camera.viewProjection);
		cubeB.TransferBuffer(camera.viewProjection);

		sprite.transform.position = { 0, 0, 0 };
		sprite.transform.scale = { 0.1f, 0.1f, 1 };
		sprite.transform.UpdateMatrix();

		hogeObj.transform.scale = { 4,4,4 };
		hogeObj.transform.UpdateMatrix();
		hogeObj.TransferBuffer(camera.viewProjection);

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

		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());

		cubeA.DrawCommands();
		cubeB.DrawCommands();
		hogeObj.DrawCommands();

		testFontImage.TransferBuffer(camera.viewProjection);
		testFontImage.DrawCommands();

		RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		//sprite.DrawCommands();
		testSprite.transform.UpdateMatrix();
		testSprite.TransferBuffer();
		testSprite.DrawCommands();
		

		//////////////////

	/*	testViewProBuff.constMap->matrix = camera.viewProjection.matrix;

		RDirectX::GetInstance()->cmdList->SetPipelineState(testPL.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(testRS.ptr.Get());
		RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &testPoint.view);
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, testViewProBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->DrawInstanced(1, 1, 0, 0);*/

		///////////////////

		//リソースバリアを表示に戻す
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();
	}
	return 0;
}