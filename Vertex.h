#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Vector3.h"

class VertexP
{
public:
	Vector3 pos;

	VertexP(Vector3 pos = { 0, 0, 0 })
		: pos(pos) {}
};

class VertexPNU
{
public:
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;

	VertexPNU(Vector3 pos = { 0, 0, 0 }, Vector3 normal = { 0, 0, 1 }, Vector2 uv = { 0, 0 })
		: pos(pos), normal(normal), uv(uv) {}

	bool operator==(const VertexPNU& a) const;

	/// <summary>
	/// 法線ベクトルを計算
	/// </summary>
	/// <param name="list">計算する頂点群</param>
	/// <param name="indexlist">インデックスリスト（三角形、全て時計回りであること）</param>
	/// <param name="indexcount">インデックスの数</param>
	static void CalcNormalVec(VertexPNU list[], const unsigned int indexlist[], const unsigned int indexcount);
};

