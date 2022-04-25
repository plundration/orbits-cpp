#pragma once

#include "raylib.h"

Vector2 operator-(const Vector2 &a);
Vector2 operator*(const Vector2 &v, float f);
Vector2 operator/(const Vector2 &v, float f);
Vector2 operator+(const Vector2 &a, const Vector2 &b);
Vector2 operator-(const Vector2 &a, const Vector2 &b);
Vector2 operator*(float f, const Vector2 &v);
Vector2 operator/(float f, const Vector2 &v);