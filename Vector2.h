#pragma once

class Vector2
{
public:
	float x;
	float y;

	//�[���x�N�g��
	Vector2(); 
	
	//�C�ӂ̐����x�N�g��
	Vector2(float x, float y); 

	~Vector2();

	//��r
	const bool operator==(const Vector2& a) const;
	const bool operator!=(const Vector2& a) const;

	//�x�N�g���̑����Z
	const Vector2 operator+(const Vector2& a) const;
	//�x�N�g���̈����Z
	const Vector2 operator-(const Vector2& a) const;
	//�t�x�N�g��
	const Vector2 operator-() const;
	//�萔�{
	const Vector2 operator*(const float a) const;
	//�萔����1�{
	const Vector2 operator/(const float a) const;

	//�x�N�g���̑����Z
	Vector2& operator+=(const Vector2& a);
	//�x�N�g���̈����Z
	Vector2& operator-=(const Vector2& a);
	//�萔�{
	Vector2& operator*=(const float a);
	//�萔����1�{
	Vector2& operator/=(const float a);
	
	//����
	float Dot(const Vector2& a) const;
	//�O��
	float Cross(const Vector2& a) const;

	//�x�N�g���̒����i����g���̂ŏd�߁j
	float Length() const;
	//�x�N�g���ׂ̂��撷���i����g��Ȃ��̂Ōy�߁j
	float LengthSq() const;

	//���K���i���̃C���X�^���X������������j
	Vector2& Normalize();
	//���K���x�N�g�����擾
	//�i���̃C���X�^���X�͏����������ɕʂ̃C���X�^���X�Ƃ��Ď擾����j
	Vector2 GetNormalize() const;
};

//���ӂ�float��u�����Ƃ���*, /���o���Ȃ��̂ł���΍�

//�萔�{
const Vector2 operator*(const float a, const Vector2& b);