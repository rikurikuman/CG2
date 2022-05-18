#pragma once

class Vector3
{
public:
	float x;
	float y;
	float z;

	//ゼロベクトル
	Vector3();

	//任意の成分ベクトル
	Vector3(float x, float y, float z);

	~Vector3();

	//比較
	bool operator==(const Vector3& a) const;
	bool operator!=(const Vector3& a) const;

	//ベクトルの足し算
	Vector3 operator+(const Vector3& a) const;
	//ベクトルの引き算
	Vector3 operator-(const Vector3& a) const;
	//逆ベクトル
	Vector3 operator-() const;
	//定数倍
	Vector3 operator*(const float a) const;
	//定数分の1倍
	Vector3 operator/(const float a) const;

	//ベクトルの足し算
	Vector3& operator+=(const Vector3& a);
	//ベクトルの引き算
	Vector3& operator-=(const Vector3& a);
	//定数倍
	Vector3& operator*=(const float a);
	//定数分の1倍
	Vector3& operator/=(const float a);

	//内積
	float Dot(const Vector3& a) const;
	//外積
	Vector3 Cross(const Vector3& a) const;

	//ベクトルの長さ（√を使うので重め）
	float Length() const;
	//ベクトルのべき乗長さ（√を使わないので軽め）
	float LengthSq() const;

	//正規化（このインスタンスを書き換える）
	Vector3& Normalize();
	//正規化ベクトルを取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Vector3 GetNormalize() const;
};

//左辺にfloatを置いたときに*, /が出来ないのでそれ対策

//定数倍
Vector3 operator*(const float a, const Vector3& b);