#include "vector.hh"

Vector2 operator-(const Vector2 &a) {
	return Vector2{ -a.x, -a.y };
}

Vector2 operator+(const Vector2 &a, const Vector2 &b) {
	return Vector2{ a.x + b.x, a.y + b.y };
}

Vector2 operator-(const Vector2 &a, const Vector2 &b) {
	return Vector2{ a.x - b.x, a.y - b.y };
}

Vector2 operator*(const Vector2 &v, float f) {
	return Vector2{ v.x * f, v.y * f };
}

Vector2 operator/(const Vector2 &v, float f) {
	return Vector2{ v.x / f, v.y / f };
}

Vector2 operator*(float f, const Vector2 &v) {
	return Vector2{ v.x * f, v.y * f };
}

Vector2 operator/(float f, const Vector2 &v) {
	return Vector2{ v.x / f, v.y / f };
}