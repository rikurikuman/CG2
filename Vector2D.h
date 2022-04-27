#pragma once

class Vector2D
{
public:
	float x;
	float y;

	//�[���x�N�g��
	Vector2D(); 
	
	//�C�ӂ̐����x�N�g��
	Vector2D(float x, float y); 

	~Vector2D();

	//��r
	const bool operator==(const Vector2D& a) const;
	const bool operator!=(const Vector2D& a) const;

	//�x�N�g���̑����Z
	const Vector2D operator+(const Vector2D& a) const;
	//�x�N�g���̈����Z
	const Vector2D operator-(const Vector2D& a) const;
	//�t�x�N�g��
	const Vector2D operator-() const;
	//�萔�{
	const Vector2D operator*(const float a) const;
	//�萔����1�{
	const Vector2D operator/(const float a) const;

	//�x�N�g���̑����Z
	Vector2D& operator+=(const Vector2D& a);
	//�x�N�g���̈����Z
	Vector2D& operator-=(const Vector2D& a);
	//�萔�{
	Vector2D& operator*=(const float a);
	//�萔����1�{
	Vector2D& operator/=(const float a);
	
	//����
	float Dot(const Vector2D& a) const;
	//�O��
	float Cross(const Vector2D& a) const;

	//�x�N�g���̒����i����g���̂ŏd�߁j
	float Length() const;
	//�x�N�g���ׂ̂��撷���i����g��Ȃ��̂Ōy�߁j
	float LengthSq() const;

	//���K���i���̃C���X�^���X������������j
	Vector2D& Normalize();
	//���K���x�N�g�����擾
	//�i���̃C���X�^���X�͏����������ɕʂ̃C���X�^���X�Ƃ��Ď擾����j
	Vector2D GetNormalize() const;
};

//���ӂ�float��u�����Ƃ���*, /���o���Ȃ��̂ł���΍�

//�萔�{
const Vector2D operator*(const float a, const Vector2D& b);