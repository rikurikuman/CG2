#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct TransformBuffer
{
	XMMATRIX matrix;
};

class Transform
{
public:
	Transform* parent = nullptr;
	XMFLOAT3 position = { 0, 0, 0 };
	XMFLOAT3 rotation = { 0, 0, 0 };
	XMFLOAT3 scale = { 1, 1, 1 };

	XMMATRIX matrix;

	//pos(0, 0, 0), rot(0, 0, 0), scale(1, 1, 1)のTransformを生成
	Transform() {
		UpdateMatrix();
	}

	Transform(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale) : position(position), rotation(rotation), scale(scale) {
		UpdateMatrix();
	}

	//メンバ変数の3情報でワールド座標変換行列を生成する
	void UpdateMatrix();

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(TransformBuffer* target);

	/// <summary>
	/// 行列と掛け算してから定数バッファに転送,
	/// ビュー+射影変換行列を入れて楽する用
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	/// <param name="matrix">転送前に掛ける行列</param>
	void Transfer(TransformBuffer* target, XMMATRIX matrix);
};

