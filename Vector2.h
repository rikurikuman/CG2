#pragma once

class Vector2
{
public:
	float x;
	float y;

	//ゼロベクトル
	Vector2(); 
	
	//任意の成分ベクトル
	Vector2(float x, float y); 

	~Vector2();

	//比較
	bool operator==(const Vector2& a) const;
	bool operator!=(const Vector2& a) const;

	//ベクトルの足し算
	Vector2 operator+(const Vector2& a) const;
	//ベクトルの引き算
	Vector2 operator-(const Vector2& a) const;
	//逆ベクトル
	Vector2 operator-() const;
	//定数倍
	Vector2 operator*(const float a) const;
	//定数分の1倍
	Vector2 operator/(const float a) const;

	//ベクトルの足し算
	Vector2& operator+=(const Vector2& a);
	//ベクトルの引き算
	Vector2& operator-=(const Vector2& a);
	//定数倍
	Vector2& operator*=(const float a);
	//定数分の1倍
	Vector2& operator/=(const float a);
	
	//内積
	float Dot(const Vector2& a) const;
	//外積
	float Cross(const Vector2& a) const;

	//ベクトルの長さ（√を使うので重め）
	float Length() const;
	//ベクトルのべき乗長さ（√を使わないので軽め）
	float LengthSq() const;

	//正規化（このインスタンスを書き換える）
	Vector2& Normalize();
	//正規化ベクトルを取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Vector2 GetNormalize() const;
};

//左辺にfloatを置いたときに*, /が出来ないのでそれ対策

//定数倍
Vector2 operator*(const float a, const Vector2& b);