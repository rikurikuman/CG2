#pragma once

class Vector3D
{
public:
	float x;
	float y;
	float z;

	//ゼロベクトル
	Vector3D();

	//任意の成分ベクトル
	Vector3D(float x, float y, float z);

	~Vector3D();

	//比較
	const bool operator==(const Vector3D& a) const;
	const bool operator!=(const Vector3D& a) const;

	//ベクトルの足し算
	const Vector3D operator+(const Vector3D& a) const;
	//ベクトルの引き算
	const Vector3D operator-(const Vector3D& a) const;
	//逆ベクトル
	const Vector3D operator-() const;
	//定数倍
	const Vector3D operator*(const float a) const;
	//定数分の1倍
	const Vector3D operator/(const float a) const;

	//ベクトルの足し算
	Vector3D& operator+=(const Vector3D& a);
	//ベクトルの引き算
	Vector3D& operator-=(const Vector3D& a);
	//定数倍
	Vector3D& operator*=(const float a);
	//定数分の1倍
	Vector3D& operator/=(const float a);

	//内積
	float Dot(const Vector3D& a) const;
	//外積
	Vector3D Cross(const Vector3D& a) const;

	//ベクトルの長さ（√を使うので重め）
	float Length() const;
	//ベクトルのべき乗長さ（√を使わないので軽め）
	float LengthSq() const;

	//正規化（このインスタンスを書き換える）
	Vector3D& Normalize();
	//正規化ベクトルを取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Vector3D GetNormalize() const;
};

//左辺にfloatを置いたときに*, /が出来ないのでそれ対策

//定数倍
const Vector3D operator*(const float a, const Vector3D& b);