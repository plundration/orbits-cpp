#include "math.hh"
#include <math.h>
#include <raylib.h>

float falloffFactor(int i_point, int n_points) {
    return (1-exp(FALLOFF_SPEED*((i_point/((float)n_points))-1)));
}

Color getStarColor(int temp) {
    u_char r, g, b;
    temp /= 100;

    if (temp <= 66) {
        //r
        r = 255;
        //g
        g = temp;
        g = 99.470802f * log(g) - 161.119f;
        if (g>255) g=255;
        else if (g<0) g=0;
        //b
        if (temp <= 19) b = 0;
        else {
            b = temp - 10;
            b = 138.5177f * log(b) - 305.04479f;
            if (b>255) b=255;
            else if (b<0) b=0;
        }
    } 
    else {
        //r
        r = temp - 60;
        r = 329.5987f * pow(r, -0.1332047f);
        if (r>255) r=255;
        else if (r<0) r=0;
        //g
        g = temp - 60;
        g = 288.1221f * pow(g, -0.0755148f);
        if (g>255) g=255;
        else if (g<0) g=0;
        //b
        b = 255;
    }

    return { r, g, b, (u_char)(255 - 20*(1-temp/350)) };
}

int randomStarTemp(int random) {
    return (random%(MAX_TEMP-MIN_TEMP)+MIN_TEMP);
}

float mag(Vector2 vec) {
    return sqrt(vec.x*vec.x+vec.y*vec.y);
}

float dot(Vector2 a, Vector2 b) {
    return (a.x*b.x+a.y*b.y);
}

float cross(Vector2 a, Vector2 b) {
    return (a.x*a.y - a.y*a.x);
}