#include "Vector3D.h"
#include <math.h>

const float EPSILON = 0.000001f;

Vector3D::Vector3D() : x(0), y(0), z(0) {}

Vector3D::Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3D::~Vector3D() {}

const bool Vector3D::operator==(const Vector3D& a) const {
	return this->x == a.x && this->y == a.y;
}

const bool Vector3D::operator!=(const Vector3D& a) const {
	return this->x != a.x || this->y != a.y;
}

const Vector3D Vector3D::operator+(const Vector3D& a) const {
	return Vector3D(this->x + a.x, this->y + a.y, this->z + a.z);
}

const Vector3D Vector3D::operator-(const Vector3D& a) const {
	return Vector3D(this->x - a.x, this->y - a.y, this->z - a.z);
}

const Vector3D Vector3D::operator-() const {
	return Vector3D(-this->x, -this->y, -this->z);
}

const Vector3D Vector3D::operator*(const float a) const {
	return Vector3D(this->x * a, this->y * a, this->z * a);
}

const Vector3D operator*(const float a, const Vector3D& b) {
	return Vector3D(b.x * a, b.y * a, b.z * a);
}

const Vector3D Vector3D::operator/(const float a) const {
	return Vector3D(this->x / a, this->y / a, this->z / a);
}

Vector3D& Vector3D::operator+=(const Vector3D& a) {
	this->x += a.x;
	this->y += a.y;
	this->z += a.z;
	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& a) {
	this->x -= a.x;
	this->y -= a.y;
	this->z -= a.z;
	return *this;
}

Vector3D& Vector3D::operator*=(const float a) {
	this->x *= a;
	this->y *= a;
	this->z *= a;
	return *this;
}

Vector3D& Vector3D::operator/=(const float a) {
	this->x /= a;
	this->y /= a;
	this->z /= a;
	return *this;
}

float Vector3D::Dot(const Vector3D& a) const {
	return this->x * a.x + this->y * a.y + this->z * a.z;
}

Vector3D Vector3D::Cross(const Vector3D& a) const {
	float x = this->y * a.z - this->z * a.y;
	float y = this->z * a.x - this->x * a.z;
	float z = this->x * a.y - this->y * a.x;
	return Vector3D(x, y, z);
}

float Vector3D::Length() const {
	return sqrt(LengthSq());
}

float Vector3D::LengthSq() const {
	return this->x * this->x + this->y * this->y + this->z * this->z;
}

Vector3D& Vector3D::Normalize() {
	float length = this->Length();
	if (length != 0) {
		this->x = this->x / length;
		this->y = this->y / length;
		this->z = this->z / length;
	}
	else {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	return *this;
}

Vector3D Vector3D::GetNormalize() const {
	float length = this->Length();
	float dx = 0;
	float dy = 0;
	float dz = 0;
	if (length != 0) {
		dx = this->x / length;
		dy = this->y / length;
		dz = this->z / length;
	}
	return Vector3D(dx, dy, dz);
}