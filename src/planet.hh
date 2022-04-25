#pragma once

#include <raylib.h>
#include <vector>

using namespace std;

struct States {
    vector<vector<Vector2>> p{}; 
    vector<vector<Vector2>> v{};
};

struct CelestialBody {
    float mass = 1;
    float radius = 1;
    Color color = RAYWHITE;
    Vector2 position = {0,0}; 
    Vector2 velocity = {0,0};
    enum Type { Star, Planet, Moon, Comet } type;
};

struct Star {
    Vector2 position = {0,0};
    Color color = RAYWHITE;
    float size;
    enum Type { Round, Pointy } type;
};