#pragma once
#include "Vector3.h"
#include <DirectXMath.h>

class Matrix4
{
private:
	class Line {
		float m[4] = {};

	public:
		float operator[](const size_t i) const;
		float& operator[](const size_t i);
	};
	Line m[4] = {};

public:
	//単位行列
	Matrix4();

	//念のためXMMARTIXからの変換
	Matrix4(DirectX::XMMATRIX matrix);

	//添え字
	Line operator[](const size_t i) const;
	Line& operator[](const size_t i);

	//逆行列
	Matrix4 operator-() const;

	//比較
	bool operator==(const Matrix4& a) const;
	bool operator!=(const Matrix4& a) const;

	//加算
	Matrix4 operator+(const Matrix4& a) const;
	//減算
	Matrix4 operator-(const Matrix4& a) const;
	//乗算
	Matrix4 operator*(const Matrix4& a) const;

	//加算
	Matrix4& operator+=(const Matrix4& a);
	//減算
	Matrix4& operator-=(const Matrix4& a);
	//乗算
	Matrix4& operator*=(const Matrix4& a);

	//転置行列(このインスタンスを書き換える
	Matrix4& Transpose();

	//転置行列を取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Matrix4 GetTranspose() const;

	//平行移動行列らくらく生成
	static Matrix4 Translation(float x, float y, float z);

	//スケーリング行列らくらく生成
	static Matrix4 Scaling(float x, float y, float z);

	//X軸回転行列らくらく生成
	static Matrix4 RotationX(float radian);
	//Y軸回転行列らくらく生成
	static Matrix4 RotationY(float radian);
	//Z軸回転行列らくらく生成
	static Matrix4 RotationZ(float radian);

	//ZXY(Roll,Pitch,Yaw)回転行列らくらく生成
	static Matrix4 RotationZXY(float radianX, float radianY, float radianZ);

	//ビュー変換行列楽々生成
	static Matrix4 View(Vector3 eye, Vector3 target, Vector3 up);

	//平行投影変換行列らくらく生成
	static Matrix4 OrthoGraphicProjection(float left, float right, float top, float bottom, float nearZ, float farZ);
	//透視投影変換行列らくらく生成
	static Matrix4 PerspectiveProjection(float fov, float aspect, float nearZ, float farZ);
};

Vector3 operator*(const Vector3 vec, const Matrix4 mat);
Vector3& operator*=(Vector3& vec, const Matrix4 mat);

