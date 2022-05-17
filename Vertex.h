#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Vector3.h"

class Vertex
{
public:
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;

	/// <summary>
	/// 法線ベクトルを計算
	/// </summary>
	/// <param name="list">計算する頂点群</param>
	/// <param name="indexlist">インデックスリスト（三角形、全て時計回りであること）</param>
	/// <param name="indexcount">インデックスの数</param>
	static void CalcNormalVec(Vertex list[], unsigned int indexlist[], unsigned int indexcount);
};

