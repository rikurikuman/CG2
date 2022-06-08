#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite(Texture* texture)
{
	this->texture = texture;

	//サイズをセットする
	this->size.x = texture->resource->GetDesc().Width;
	this->size.y = texture->resource->GetDesc().Height;

	Init();
}

Sprite::Sprite(Texture* texture, Vector2 size)
{
	this->texture = texture;

	//サイズをセットする
	this->size.x = size.x;
	this->size.y = size.y;

	Init();
}

void Sprite::Init()
{
	//頂点データ
	Vertex vertices[] = {
		{{ -0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{ 0.5f * size.x, 0.5f * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{ 0.5f * size.x, -0.5f * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	Vertex::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, GetRWindow()->GetWidth(),
		0.0f, GetRWindow()->GetHeight(),
		0.0f, 1.0f
	);
	
	viewProjectionBuff.constMap->matrix = matProjection;
}

void Sprite::DrawCommands()
{
	//頂点バッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetVertexBuffers(0, 1, &vertBuff.view);

	//インデックスバッファビューの設定コマンド
	GetRDirectX()->cmdList->IASetIndexBuffer(&indexBuff.view);

	//定数バッファビューの設定コマンド
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	GetRDirectX()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	GetRDirectX()->cmdList->SetGraphicsRootDescriptorTable(0, texture->gpuHandle);

	TransferBuffer();

	//描画コマンド
	GetRDirectX()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void SpriteManager::Init()
{
	rootSignature = GetRDirectX()->rootSignature;

	//Basicシェーダ達の読み込みとコンパイル
	ComPtr<ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

	// グラフィックスパイプライン設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// シェーダーの設定
	pipelineState.desc.VS.pShaderBytecode = GetRDirectX()->basicVS.shaderBlob->GetBufferPointer();
	pipelineState.desc.VS.BytecodeLength = GetRDirectX()->basicVS.shaderBlob->GetBufferSize();
	pipelineState.desc.PS.pShaderBytecode = GetRDirectX()->basicPS.shaderBlob->GetBufferPointer();
	pipelineState.desc.PS.BytecodeLength = GetRDirectX()->basicPS.shaderBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineState.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //標準

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	// ブレンドステート(半透明合成)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineState.desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// 頂点レイアウトの設定
	pipelineState.desc.InputLayout.pInputElementDescs = inputLayout;
	pipelineState.desc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineState.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineState.desc.NumRenderTargets = 1;
	pipelineState.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineState.desc.SampleDesc.Count = 1; //1ピクセルにつき1回サンプリング

	//セット
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}
