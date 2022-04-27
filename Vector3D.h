#pragma once

class Vector3D
{
public:
	float x;
	float y;
	float z;

	//�[���x�N�g��
	Vector3D();

	//�C�ӂ̐����x�N�g��
	Vector3D(float x, float y, float z);

	~Vector3D();

	//��r
	const bool operator==(const Vector3D& a) const;
	const bool operator!=(const Vector3D& a) const;

	//�x�N�g���̑����Z
	const Vector3D operator+(const Vector3D& a) const;
	//�x�N�g���̈����Z
	const Vector3D operator-(const Vector3D& a) const;
	//�t�x�N�g��
	const Vector3D operator-() const;
	//�萔�{
	const Vector3D operator*(const float a) const;
	//�萔����1�{
	const Vector3D operator/(const float a) const;

	//�x�N�g���̑����Z
	Vector3D& operator+=(const Vector3D& a);
	//�x�N�g���̈����Z
	Vector3D& operator-=(const Vector3D& a);
	//�萔�{
	Vector3D& operator*=(const float a);
	//�萔����1�{
	Vector3D& operator/=(const float a);

	//����
	float Dot(const Vector3D& a) const;
	//�O��
	Vector3D Cross(const Vector3D& a) const;

	//�x�N�g���̒����i����g���̂ŏd�߁j
	float Length() const;
	//�x�N�g���ׂ̂��撷���i����g��Ȃ��̂Ōy�߁j
	float LengthSq() const;

	//���K���i���̃C���X�^���X������������j
	Vector3D& Normalize();
	//���K���x�N�g�����擾
	//�i���̃C���X�^���X�͏����������ɕʂ̃C���X�^���X�Ƃ��Ď擾����j
	Vector3D GetNormalize() const;
};

//���ӂ�float��u�����Ƃ���*, /���o���Ȃ��̂ł���΍�

//�萔�{
const Vector3D operator*(const float a, const Vector3D& b);