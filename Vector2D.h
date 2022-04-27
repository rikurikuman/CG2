#pragma once

class Vector2D
{
public:
	float x;
	float y;

	//ゼロベクトル
	Vector2D(); 
	
	//任意の成分ベクトル
	Vector2D(float x, float y); 

	~Vector2D();

	//比較
	const bool operator==(const Vector2D& a) const;
	const bool operator!=(const Vector2D& a) const;

	//ベクトルの足し算
	const Vector2D operator+(const Vector2D& a) const;
	//ベクトルの引き算
	const Vector2D operator-(const Vector2D& a) const;
	//逆ベクトル
	const Vector2D operator-() const;
	//定数倍
	const Vector2D operator*(const float a) const;
	//定数分の1倍
	const Vector2D operator/(const float a) const;

	//ベクトルの足し算
	Vector2D& operator+=(const Vector2D& a);
	//ベクトルの引き算
	Vector2D& operator-=(const Vector2D& a);
	//定数倍
	Vector2D& operator*=(const float a);
	//定数分の1倍
	Vector2D& operator/=(const float a);
	
	//内積
	float Dot(const Vector2D& a) const;
	//外積
	float Cross(const Vector2D& a) const;

	//ベクトルの長さ（√を使うので重め）
	float Length() const;
	//ベクトルのべき乗長さ（√を使わないので軽め）
	float LengthSq() const;

	//正規化（このインスタンスを書き換える）
	Vector2D& Normalize();
	//正規化ベクトルを取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Vector2D GetNormalize() const;
};

//左辺にfloatを置いたときに*, /が出来ないのでそれ対策

//定数倍
const Vector2D operator*(const float a, const Vector2D& b);