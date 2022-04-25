#include <algorithm>
#include <math.h>
#include <raylib.h>
#include <sys/types.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <raymath.h>

#include "planet.hh"
#include "vector.hh"
#include "serialiser.hh"
#include "math.hh"

#define SCREEN_WIDTH (1200)
#define SCREEN_HEIGHT (1200)
#define WINDOW_TITLE "Orbits"

const int POINT_COUNT = 8000;
const int PAST_POINT_COUNT = 700;
const int LINE_TRANSPARENCY = 120;
const float VELOCITY_DELTA_TIME = 1.5f;
const float CAMERA_SPEED = 2.0f;
const float TIME_CONSTANT = 7.0f;

using namespace std;

void updateBodies(vector<CelestialBody>& bodies, float time);
void updateFuturePath(vector<CelestialBody>& bodies, States &futurePoints, float time);
States predictPath(vector<CelestialBody> bodies, int pointCount, float deltaTime);
void drawPointPath(vector<Vector2>& points, Vector2 start, Color color);
void drawPointPathRelative(vector<Vector2>& points, vector<Vector2>& relativeToPoints, Vector2 start, Vector2 relativeStart, Color color);
void drawPointPaths(vector<vector<Vector2>> &points, vector<CelestialBody> &bodies, bool centered, int i_targetBody);
vector<Vector2> getOrbitPoints(CelestialBody sun, CelestialBody planet);
inline void updatePastPoints(vector<vector<Vector2>> &points, vector<CelestialBody> &bodies);

int main() 
{
    // Seed random number generator
    srand(time(0));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
    
    Camera2D camera = {
        .offset = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f},
        .target = {0,0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    
    Texture2D cometTexture = LoadTexture("comet.png");
    Rectangle src = {0,0,(float) cometTexture.width, (float) cometTexture.height};

    float timeMultiplier = TIME_CONSTANT;
    float wantedDeltaTime = 0.01f;
    float leftoverDelta = 0.0f;

    bool centered = true;
    bool drawRays = true;
    bool plotPastPath = true;
    int i_targetBody = 0;
    
    vector<CelestialBody> bodies = Deserialize("scene.yaml");
    vector<vector<Vector2>> pastPoints = vector<vector<Vector2>>(bodies.size());
    updatePastPoints(pastPoints, bodies);
    States futurePath = predictPath(bodies, POINT_COUNT, wantedDeltaTime);

    // Generate stars
    vector<Star> stars{};
    for (int i = 0; i < 90; i++) {
        float x = rand() / static_cast<float>(RAND_MAX) * SCREEN_WIDTH;
        float y = rand() / static_cast<float>(RAND_MAX) * SCREEN_HEIGHT;
        int temp = randomStarTemp(rand());
        Star star{
            .position = {x,y},
            .color = getStarColor(temp),
            .size = (float) (rand()%2+1),
            .type = Star::Type::Round
        };
        stars.push_back(star);
    }

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime()*timeMultiplier + leftoverDelta;
        
        if (GetFrameTime() > 1.0f) timeMultiplier = TIME_CONSTANT;

        // Update bodies
        {
            int ratio = deltaTime/wantedDeltaTime;
            if (ratio >= 1) {
                for (int n = 0; n < ratio; n++) { 
                    updateFuturePath(bodies, futurePath, wantedDeltaTime);
                    if (n%9!=8) continue;
                    updatePastPoints(pastPoints, bodies);
                }
                leftoverDelta = deltaTime - wantedDeltaTime*ratio;
                updatePastPoints(pastPoints, bodies);
            }
            else { 
                leftoverDelta = deltaTime;
            }
        }

        // Update controls
        {
            // Camera control
            if (IsKeyPressed(KEY_C)) centered = !centered;
            if (IsKeyPressed(KEY_R)) drawRays = !drawRays;
            if (IsKeyPressed(KEY_M)) plotPastPath = !plotPastPath;
            if (IsKeyPressed(KEY_N)) {
                i_targetBody++;
                i_targetBody %= bodies.size();
            }
            if (IsKeyPressed(KEY_P)) {
                if (i_targetBody == 0) i_targetBody = bodies.size() - 1;
                else i_targetBody--;
            }

            if (centered) camera.target = bodies[i_targetBody].position;
            else {
                if (IsKeyDown(KEY_W)) camera.target.y -= CAMERA_SPEED/camera.zoom;
                if (IsKeyDown(KEY_S)) camera.target.y += CAMERA_SPEED/camera.zoom;
                if (IsKeyDown(KEY_D)) camera.target.x += CAMERA_SPEED/camera.zoom;
                if (IsKeyDown(KEY_A)) camera.target.x -= CAMERA_SPEED/camera.zoom;
            }
            camera.zoom *= 1+GetMouseWheelMove()/30.0f;

            if (IsKeyPressed(KEY_EQUAL)) timeMultiplier *= 1.6f;
            if (IsKeyPressed(KEY_MINUS)) timeMultiplier /= 1.6f;
        }


        BeginDrawing();
            ClearBackground(BLACK);

            // Draw stars
            for (int i = 0; i < stars.size(); i++) DrawCircleV(stars[i].position, stars[i].size, stars[i].color);

            BeginMode2D(camera);
                // Draw paths
                if (plotPastPath) drawPointPaths(pastPoints, bodies, centered, i_targetBody);
                else  { drawPointPaths(futurePath.p, bodies, centered, i_targetBody); }
                
                // Draw rays
                if (drawRays) {
                    if (centered) 
                        for (int i = 0; i < bodies.size(); i++)
                            DrawLineV(bodies[i].position, bodies[i].position + (bodies[i].velocity - bodies[i_targetBody].velocity)*VELOCITY_DELTA_TIME, bodies[i].color);
                    else
                        for (int i = 0; i < bodies.size(); i++)
                            DrawLineV(bodies[i].position, bodies[i].position + bodies[i].velocity*VELOCITY_DELTA_TIME, bodies[i].color);
                }

                // Draw celestial bodies
                for (int i = 0; i < bodies.size(); i++) {
                    Color color = bodies[i].color;
                    int radius = bodies[i].radius*camera.zoom >= 2 ? bodies[i].radius : 2/camera.zoom;
                    DrawCircleV(bodies[i].position, radius, color);
                    if (bodies[i].type == CelestialBody::Type::Comet) {
                        CelestialBody &body = bodies[i];
                        float scale = .12f;
                        Rectangle dest = {body.position.x, body.position.y, cometTexture.width*scale, cometTexture.height*scale};
                        Vector2 dir = bodies[0].position - body.position;
                        float rot = Vector2Angle({1,0}, dir) + 180.0f;
                        DrawTexturePro(cometTexture, src, dest, {44*scale,38*scale}, rot, WHITE);
                    }
                }
            EndMode2D();
            
            // Draw text
            DrawText("C - center view; N/P - switch bodies", 5, 5, 25, RAYWHITE);
            DrawText("R - draw velocity; M - toggle past and future", 5, 35, 25, RAYWHITE);
            DrawText("'-' to slow time, '=' to speed up", 5, 65, 25, RAYWHITE);
            DrawText("WASD to move, scroll to zoom", 5, 95, 25, RAYWHITE);
            
            DrawText(plotPastPath ? "PAST" : "FUTURE", 5, SCREEN_HEIGHT - 70, 30, RAYWHITE);
            const char* speed = timeMultiplier >= TIME_CONSTANT ? TextFormat("x%d speed", (int)(timeMultiplier/TIME_CONSTANT)) : TextFormat("x1/%d speed", (int)(TIME_CONSTANT/timeMultiplier));
            DrawText(speed, 5, SCREEN_HEIGHT - 35, 30, RAYWHITE);
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void updateBodies(vector<CelestialBody>& bodies, float time) {
    for (int i = 0; i < bodies.size(); i++) {
        CelestialBody &body_i = bodies[i];

        for (int j = i+1; j < bodies.size(); j++) {
            CelestialBody &body_j = bodies[j];
            
            float dist_x = body_i.position.x-body_j.position.x;
            float dist_y = body_i.position.y-body_j.position.y;
            float dist_sq = dist_x*dist_x+dist_y*dist_y;
            
            Vector2 to_i = {dist_x, dist_y};
            to_i = to_i / sqrt(dist_sq);

            Vector2 acceleration_per_mass = to_i*G/dist_sq;
            
            body_i.velocity = body_i.velocity + acceleration_per_mass*body_j.mass*time;
            body_j.velocity = body_j.velocity + acceleration_per_mass*body_i.mass*time;
        }
        
        body_i.position = body_i.position + body_i.velocity*time;
    }
}

void updateFuturePath(vector<CelestialBody>& bodies, States &futurePoints, float time) {
    int lastPoint = futurePoints.p[0].size()-1;
    for (int i = 0; i < bodies.size(); i++) {
        // Init body at last point
        CelestialBody body_i_last = bodies[i];
        body_i_last.position = futurePoints.p[i][lastPoint];
        body_i_last.velocity = futurePoints.v[i][lastPoint];
        // ---

        for (int j = i+1; j < bodies.size(); j++) {
            CelestialBody body_j_last = bodies[j];
            body_j_last.position = futurePoints.p[j][lastPoint];
            body_j_last.velocity = futurePoints.v[j][lastPoint];
            
            float dist_x = body_i_last.position.x-body_j_last.position.x;
            float dist_y = body_i_last.position.y-body_j_last.position.y;
            float dist_sq = dist_x*dist_x+dist_y*dist_y;
            
            Vector2 to_i = {dist_x, dist_y};
            to_i = to_i / sqrt(dist_sq);

            Vector2 acceleration_per_mass = to_i*G/dist_sq;
            
            body_i_last.velocity = body_i_last.velocity + acceleration_per_mass*body_j_last.mass*time;
            futurePoints.v[j][lastPoint] = body_j_last.velocity + acceleration_per_mass*body_i_last.mass*time;
        }
        
        body_i_last.position = body_i_last.position + body_i_last.velocity*time;
        futurePoints.p[i].push_back(body_i_last.position);
        futurePoints.v[i].push_back(body_i_last.velocity);
        bodies[i].position = futurePoints.p[i][0];
        bodies[i].velocity = futurePoints.v[i][0];
        futurePoints.p[i].erase(futurePoints.p[i].begin());
        futurePoints.v[i].erase(futurePoints.v[i].begin());
    }
}

States predictPath(vector<CelestialBody> bodies, int pointCount, float deltaTime) {
    int size = bodies.size();
    States states = States{
        .p = vector<vector<Vector2>>(size),
        .v = vector<vector<Vector2>>(size)
    };

    for (int i = 0; i < pointCount; i++) {
        updateBodies(bodies, deltaTime);
        for (int b = 0; b < size; b++) {
            states.p[b].push_back(bodies[b].position);
            states.v[b].push_back(bodies[b].velocity);
        }
    }    

    return states;
}

void drawPointPath(vector<Vector2>& points, Vector2 start, Color color) {
    color.a = LINE_TRANSPARENCY;
    int size = points.size();
    DrawLineV(start, points[0], color);
    for (int p = 1; p < size; p++) {
        color.a = LINE_TRANSPARENCY * falloffFactor(p, size);
        DrawLineV(points[p-1], points[p], color);
    }
}

void drawPointPathRelative(vector<Vector2>& points, vector<Vector2>& relativeToPoints, Vector2 start, Vector2 relativeStart, Color color) {
    color.a = LINE_TRANSPARENCY;
    int size = points.size();
    DrawLineV(start, points[0] - (relativeToPoints[0] - relativeStart), color);
    for (int p = 1; p < size; p++) {
        color.a = LINE_TRANSPARENCY * falloffFactor(p, size);
        DrawLineV(points[p-1] - (relativeToPoints[p-1] - relativeStart), points[p] - (relativeToPoints[p] - relativeStart), color                         );
    }
}

void drawPointPaths(vector<vector<Vector2>> &points, vector<CelestialBody> &bodies, bool centered, int i_targetBody) {
    if (centered) 
        for (int i = 0; i < bodies.size(); i++) 
            drawPointPathRelative(points[i], points[i_targetBody], bodies[i].position, bodies[i_targetBody].position, bodies[i].color);
    else 
        for (int i = 0; i < bodies.size(); i++) 
            drawPointPath(points[i], bodies[i].position, bodies[i].color);
}

vector<Vector2> getOrbitPoints(CelestialBody sun, CelestialBody planet) {
    float e, p, a;
    float mu = G*sun.mass;
    Vector2 v = planet.velocity;
    Vector2 r = planet.position - sun.position;
    float h = cross(r,v);

    Vector2 e_vec = (r*(pow(mag(v),2)-mu/mag(r))-v*dot(r,v))/mu;
    e = mag(e_vec);
    
    float energy = pow(mag(v),2)/2-mu/mag(r);
    
    if (abs(e-1.0f) > 0.0000000001f) {
        a = -mu/2/energy;
        p = a*(1-e*e);
    } else {
        p = h*h/mu;
    }
    
    float s = (r.x*v.y-r.y*v.x)*(r.x*v.x+r.y*v.y)/(r.x*r.x+r.y*r.y) >= 0 ? 1 : -1;
    
    float deltaO = s/cos((p-mag(r))/(e*mag(r)))-atan2(r.y,r.x);

    vector<Vector2> points{};

    const int point_count = 100;
    for (int i = 0; i < point_count; i++) {
        float theta = 2.0f*M_PI*i/point_count;
        float r = p/(1+e*cos(theta));
        float x = r*cos(theta + deltaO);
        float y = -r*sin(theta + deltaO);
        points.push_back({x,y});
    }

    return points;
}

inline void updatePastPoints(vector<vector<Vector2>> &points, vector<CelestialBody> &bodies) {
    for (int i = 0; i < bodies.size(); i++) {
        points[i].insert(points[i].begin(), bodies[i].position);
        if (points[i].size() > PAST_POINT_COUNT) points[i].pop_back();
    }
}