#include "Matrix4.h"
#include <stdexcept>
#include "Util.h"

const float EPSILON = 0.000001f;

float Matrix4::Line::operator[](const size_t i) const
{
	if (i >= 4) {
		throw std::out_of_range("4—ñ‚È‚Ì‚É5—ñ–ÚˆÈ~‚ğG‚é‚È");
	}
	return m[i];
}

float& Matrix4::Line::operator[](const size_t i)
{
	if (i >= 4) {
		throw std::out_of_range("4—ñ‚È‚Ì‚É5—ñ–ÚˆÈ~‚ğG‚é‚È");
	}
	return m[i];
}

Matrix4::Matrix4()
{
	m[0][0] = 1;
	m[1][1] = 1;
	m[2][2] = 1;
	m[3][3] = 1;
}

Matrix4::Matrix4(DirectX::XMMATRIX matrix)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = matrix.r[i].m128_f32[j];
		}
	}
}

Matrix4::Line Matrix4::operator[](const size_t i) const
{
	if (i >= 4) {
		throw std::out_of_range("4s4—ñ‚È‚Ì‚É5s–ÚˆÈ~‚ğG‚é‚È");
	}
	return m[i];
}

Matrix4::Line& Matrix4::operator[](const size_t i)
{
	if (i >= 4) {
		throw std::out_of_range("4s4—ñ‚È‚Ì‚É5s–ÚˆÈ~‚ğG‚é‚È");
	}
	return m[i];
}

Matrix4 Matrix4::operator-() const
{
	Matrix4 temp;
	float mat[4][8] = {0};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = m[i][j];
		}
	}

	mat[0][4] = 1;
	mat[1][5] = 1;
	mat[2][6] = 1;
	mat[3][7] = 1;

	for (int n = 0; n < 4; n++) {
		//Å‘å‚Ìâ‘Î’l‚ğ’Tõ‚·‚é(‚Æ‚è‚ ‚¦‚¸‘ÎÛ¬•ª‚ğÅ‘å‚Æ‰¼’è‚µ‚Ä‚¨‚­)
		float max = abs(mat[n][n]);
		int maxIndex = n;

		for (int i = n + 1; i < 4; i++) {
			if (abs(mat[i][n]) > max) {
				max = abs(mat[i][n]);
				maxIndex = i;
			}
		}

		//Å‘å‚Ìâ‘Î’l‚ª0‚¾‚Á‚½‚ç‹ts—ñ‚Í‹‚ß‚ç‚ê‚È‚¢
		if (abs(mat[maxIndex][n]) <= EPSILON) {
			return temp; //‚Æ‚è‚ ‚¦‚¸’PˆÊs—ñ•Ô‚µ‚¿‚á‚¤
		}

		//“ü‚ê‘Ö‚¦
		if (n != maxIndex) {
			for (int i = 0; i < 8; i++) {
				float f = mat[maxIndex][i];
				mat[maxIndex][i] = mat[n][i];
				mat[n][i] = f;
			}
		}

		//Š|‚¯‚½‚ç1‚É‚È‚é’l‚ğZo
		float mul = 1 / mat[n][n];

		//Š|‚¯‚é
		for (int i = 0; i < 8; i++) {
			mat[n][i] *= mul;
		}

		//‘¼‘S•”0‚É‚·‚é
		for (int i = 0; i < 4; i++) {
			if (n == i) {
				continue;
			}

			float mul = -mat[i][n];

			for (int j = 0; j < 8; j++) {
				mat[i][j] += mat[n][j] * mul;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i][j] = mat[i][j + 4];
		}
	}

	return temp;
}

bool Matrix4::operator==(const Matrix4& a) const
{
	bool ok = true;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (m[i][j] != a[i][j]) {
				ok = false;
			}
		}
	}
	return ok;
}

bool Matrix4::operator!=(const Matrix4& a) const
{
	return !(*this == a);
}

Matrix4 Matrix4::operator+(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp += a;
	return temp;
}

Matrix4 Matrix4::operator-(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp -= a;
	return temp;
}

Matrix4 Matrix4::operator*(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp *= a;
	return temp;
}

Matrix4& Matrix4::operator+=(const Matrix4& a)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] += a[i][j];
		}
	}
	return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& a)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] -= a[i][j];
		}
	}
	return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& a)
{
	Matrix4 temp(*this);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float f = 0;
			for (int k = 0; k < 4; k++) {
				f += temp.m[i][k] * a[k][j];
			}
			m[i][j] = f;
		}
	}
	return *this;
}

Matrix4& Matrix4::Transpose()
{
	for (int i = 0; i < 4; i++) {
		for (int j = i; j < 4; j++) {
			float f = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = f;
		}
	}

	return *this;
}

Matrix4 Matrix4::GetTranspose() const
{
	Matrix4 temp(*this);
	temp.Transpose();
	return temp;
}

Matrix4 Matrix4::Translation(float x, float y, float z)
{
	Matrix4 mat;
	mat[3][0] = x;
	mat[3][1] = y;
	mat[3][2] = z;
	return mat;
}

Matrix4 Matrix4::Scaling(float x, float y, float z)
{
	Matrix4 mat;
	mat[0][0] = x;
	mat[1][1] = y;
	mat[2][2] = z;
	return mat;
}

Matrix4 Matrix4::RotationX(float radian)
{
	Matrix4 mat;
	mat[1][1] = cosf(radian);
	mat[1][2] = sinf(radian);
	mat[2][1] = -sinf(radian);
	mat[2][2] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationY(float radian)
{
	Matrix4 mat;
	mat[0][0] = cosf(radian);
	mat[0][2] = -sinf(radian);
	mat[2][0] = sinf(radian);
	mat[2][2] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationZ(float radian)
{
	Matrix4 mat;
	mat[0][0] = cosf(radian);
	mat[0][1] = sinf(radian);
	mat[1][0] = -sinf(radian);
	mat[1][1] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationZXY(float radianX, float radianY, float radianZ)
{
	Matrix4 mat;
	mat *= RotationZ(radianZ);
	mat *= RotationX(radianX);
	mat *= RotationY(radianY);
	return mat;
}

Matrix4 Matrix4::View(Vector3 eye, Vector3 target, Vector3 up)
{
	Matrix4 mat;

	Vector3 direction = target - eye;
	direction.Normalize();

	Vector3 xVec = up.Cross(direction);
	xVec.Normalize();
	Vector3 yVec = direction.Cross(xVec);
	yVec.Normalize();

	mat[0][0] = xVec.x;
	mat[0][1] = xVec.y;
	mat[0][2] = xVec.z;
	mat[1][0] = yVec.x;
	mat[1][1] = yVec.y;
	mat[1][2] = yVec.z;
	mat[2][0] = direction.x;
	mat[2][1] = direction.y;
	mat[2][2] = direction.z;
	mat[3][0] = eye.x;
	mat[3][1] = eye.y;
	mat[3][2] = eye.z;

	mat = -mat;
	return mat;
}

Matrix4 Matrix4::OrthoGraphicProjection(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	Matrix4 mat;

	float width = 1.0f / (right - left);
	float height = 1.0f / (top - bottom);
	float range = 1.0f / (farZ - nearZ);

	mat[0][0] = width * 2;
	mat[1][1] = height * 2;
	mat[2][2] = range;

	mat[3][0] = -(left + right) * width;
	mat[3][1] = -(top + bottom) * height;
	mat[3][2] = -range * nearZ;
	mat[3][3] = 1.0f;

	return mat;
}

Matrix4 Matrix4::PerspectiveProjection(float fov, float aspect, float nearZ, float farZ)
{
	Matrix4 mat;

	Matrix4 _a; //ƒAƒXƒyƒNƒg”ä‚ğ1:1‚É
	_a[0][0] = 1 / aspect;

	Matrix4 _b; //’¼•û‘Ì‚É
	_b[0][0] = 1 / tanf(fov / 2);
	_b[1][1] = 1 / tanf(fov / 2);

	Matrix4 _c; //Œ´“_‚É‚­‚Á‚Â‚¯‚Äk‚ß‚é
	_c[2][2] = farZ * (1 / (farZ - nearZ));
	_c[3][2] = farZ * (-nearZ / (farZ - nearZ));

	mat = _a * _b * _c;
	mat[2][3] = 1;
	mat[3][3] = 0;

	return mat;
}
