#pragma once
#include <DirectXMath.h>
#include <d3d12.h>

class Vertex
{
public:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;

	/// <summary>
	/// 法線ベクトルを計算
	/// </summary>
	/// <param name="list">計算する頂点群</param>
	/// <param name="indexlist">インデックスリスト（三角形、全て時計回りであること）</param>
	/// <param name="indexcount">インデックスの数</param>
	static void CalcNormalVec(Vertex list[], uint16_t indexlist[], uint16_t indexcount);
};

