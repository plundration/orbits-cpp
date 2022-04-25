#pragma once

#include "raylib.h"
#include <cmath>

const float G = 66.743f;
const float FALLOFF_SPEED = 6.2f;
const u_int MIN_TEMP = 1000;
const u_int MAX_TEMP = 30000; // 40000

float falloffFactor(int i_point, int n_points);
Color getStarColor(int temp);
int randomStarTemp(int random);
float mag(Vector2 vec);
float dot(Vector2 a, Vector2 b);
float cross(Vector2 a, Vector2 b);