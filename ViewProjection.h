#pragma once
#include <DirectXMath.h>

class ViewProjection
{
public:
	DirectX::XMMATRIX matrix;
	DirectX::XMFLOAT3 eye = {0, 0, 0}; //視点座標
	DirectX::XMFLOAT3 target = {0, 0, 1}; //注視点座標
	DirectX::XMFLOAT3 up = {0, 1, 0}; //上方向ベクトル

	float fov = DirectX::XMConvertToRadians(45); //画角(Rad)
	float aspect = 1; //アスペクト比(横/縦)
	float nearclip = 0.1f; //ニアクリップ距離
	float farclip = 1000; //ファークリップ距離

	//メンバ変数の情報で各行列を生成する
	void UpdateMatrix();
};