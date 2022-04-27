#include "Vector2D.h"
#include <math.h>

const float EPSILON = 0.000001f;

Vector2D::Vector2D() : x(0), y(0) {}

Vector2D::Vector2D(float x, float y) : x(x), y(y) {}

Vector2D::~Vector2D() {}

const bool Vector2D::operator==(const Vector2D& a) const {
	return this->x == a.x && this->y == a.y;
}

const bool Vector2D::operator!=(const Vector2D& a) const {
	return this->x != a.x || this->y != a.y;
}

const Vector2D Vector2D::operator+(const Vector2D& a) const {
	return Vector2D(this->x + a.x, this->y + a.y);
}

const Vector2D Vector2D::operator-(const Vector2D& a) const {
	return Vector2D(this->x - a.x, this->y - a.y);
}

const Vector2D Vector2D::operator-() const {
	return Vector2D(-this->x, -this->y);
}

const Vector2D Vector2D::operator*(const float a) const {
	return Vector2D(this->x * a, this->y * a);
}

const Vector2D operator*(const float a, const Vector2D& b) {
	return Vector2D(b.x * a, b.y * a);
}

const Vector2D Vector2D::operator/(const float a) const {
	return Vector2D(this->x / a, this->y / a);
}

Vector2D& Vector2D::operator+=(const Vector2D& a) {
	this->x += a.x;
	this->y += a.y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& a) {
	this->x -= a.x;
	this->y -= a.y;
	return *this;
}

Vector2D& Vector2D::operator*=(const float a) {
	this->x *= a;
	this->y *= a;
	return *this;
}

Vector2D& Vector2D::operator/=(const float a) {
	this->x /= a;
	this->y /= a;
	return *this;
}

float Vector2D::Dot(const Vector2D& a) const {
	return this->x * a.x + this->y * a.y;
}

float Vector2D::Cross(const Vector2D& a) const {
	return this->x * a.y - a.x * this->y;
}

float Vector2D::Length() const {
	return sqrt(LengthSq());
}

float Vector2D::LengthSq() const {
	return this->x * this->x + this->y * this->y;
}

Vector2D& Vector2D::Normalize() {
	float length = this->Length();
	if (length != 0) {
		this->x = this->x / length;
		this->y = this->y / length;
	}
	else {
		this->x = 0;
		this->y = 0;
	}
	return *this;
}

Vector2D Vector2D::GetNormalize() const {
	float length = this->Length();
	float dx = 0;
	float dy = 0;
	if (length != 0) {
		dx = this->x / length;
		dy = this->y / length;
	}
	return Vector2D(dx, dy);
}