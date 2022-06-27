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

	//���낢��

	Shader::Register("TestVS", Shader("Shader/TestVS.hlsl", "main", "vs_5_0"));
	Shader::Register("TestGS", Shader("Shader/TestGS.hlsl", "main", "gs_5_0"));
	Shader::Register("TestPS", Shader("Shader/TestPS.hlsl", "main", "ps_5_0"));

	RootSignature testRS = GetRDirectX()->rootSignature;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(2);
	//�e�N�X�`�����W�X�^0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@1��(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[1].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	testRS.desc.RootParamaters = rootParams;
	testRS.Create();

	GraphicsPipeline testPL = GetRDirectX()->pipelineState;
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

	float angle = 0.0f; //�J�����̉�]�p

	DebugCamera camera({0, 0, -10});

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
			cubeA.transform.position.z += 0.1f;
		}
		if (GetKey(DIK_DOWN)) {
			cubeA.transform.position.z -= 0.1f;
		}
		if (GetKey(DIK_RIGHT)) {
			cubeA.transform.position.x += 0.1f;
		}
		if (GetKey(DIK_LEFT)) {
			cubeA.transform.position.x -= 0.1f;
		}

		if (GetKey(DIK_NUMPAD8)) {
			cubeA.transform.scale.y += 0.1f;
		}
		if (GetKey(DIK_NUMPAD2)) {
			cubeA.transform.scale.y -= 0.1f;
		}
		if (GetKey(DIK_NUMPAD6)) {
			cubeA.transform.scale.x += 0.1f;
		}
		if (GetKey(DIK_NUMPAD4)) {
			cubeA.transform.scale.x -= 0.1f;
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

		//�ȉ��`��
		//�o�b�N�o�b�t�@�ԍ��̎擾
		UINT bbIndex = GetRDirectX()->swapChain->GetCurrentBackBufferIndex();

		//���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = GetRDirectX()->backBuffers[bbIndex].Get();
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:�\������
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:�`���
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		//�o�b�N�o�b�t�@��`���ɂ���(�����_�[�^�[�Q�b�g�̐ݒ�)
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRDirectX()->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * GetRDirectX()->device->GetDescriptorHandleIncrementSize(GetRDirectX()->rtvHeap->GetDesc().Type);

		//�[�x�X�e���V�����ݒ�
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetRDirectX()->dsvHeap->GetCPUDescriptorHandleForHeapStart();

		//�Z�b�g
		GetRDirectX()->cmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		//��ʃN���A�`
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; //���ۂ��F�ŃN���A����
		if (GetKey(DIK_SPACE)) {
			//�Ԃ��ۂ��F�ŃN���A����
			clearColor[0] = 0.5f;
			clearColor[1] = 0.1f;
			clearColor[2] = 0.1f;
			clearColor[3] = 0.0f;
		}
		GetRDirectX()->cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		GetRDirectX()->cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//�`��R�}���h
		//�r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = WIN_WIDTH;
		viewport.Height = WIN_HEIGHT;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		GetRDirectX()->cmdList->RSSetViewports(1, &viewport);

		//�V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + WIN_WIDTH;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + WIN_HEIGHT;
		GetRDirectX()->cmdList->RSSetScissorRects(1, &scissorRect);

		GetRDirectX()->cmdList->SetPipelineState(GetRDirectX()->pipelineState.ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(GetRDirectX()->rootSignature.ptr.Get());

		//�v���~�e�B�u�`��ݒ�
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRV�q�[�v�̐ݒ�R�}���h
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		GetRDirectX()->cmdList->SetDescriptorHeaps(1, &_heap);

		//�`��R�}���h

		GetRDirectX()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

		sprite.DrawCommands();

		GetRDirectX()->cmdList->SetPipelineState(GetRDirectX()->pipelineState.ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(GetRDirectX()->rootSignature.ptr.Get());

		cubeA.DrawCommands();
		cubeB.DrawCommands();
		hogeObj.DrawCommands();

		//////////////////

		testViewProBuff.constMap->matrix = camera.viewProjection.matrix;

		GetRDirectX()->cmdList->SetPipelineState(testPL.ptr.Get());
		GetRDirectX()->cmdList->SetGraphicsRootSignature(testRS.ptr.Get());
		GetRDirectX()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &testPoint.view);
		GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, testViewProBuff.constBuff->GetGPUVirtualAddress());
		GetRDirectX()->cmdList->DrawInstanced(1, 1, 0, 0);

		///////////////////

		//���\�[�X�o���A��\���ɖ߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:�`�悩��
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:�\����
		GetRDirectX()->cmdList->ResourceBarrier(1, &barrierDesc);

		EndFrame();
	}
	return 0;
}