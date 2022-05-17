#pragma once
#include "Matrix4.h"
#include "Util.h"

struct ViewProjectionBuffer
{
	Matrix4 matrix;
};

class ViewProjection
{
public:
	Matrix4 matrix;
	Vector3 eye = {0, 0, 0}; //視点座標
	Vector3 target = {0, 0, 1}; //注視点座標
	Vector3 up = {0, 1, 0}; //上方向ベクトル

	float fov = Util::AngleToRadian(45); //画角(Rad)
	float aspect = 1; //アスペクト比(横/縦)
	float nearclip = 0.1f; //ニアクリップ距離
	float farclip = 1000; //ファークリップ距離

	//メンバ変数の情報で各行列を生成する
	void UpdateMatrix();

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(ViewProjectionBuffer* target);
};