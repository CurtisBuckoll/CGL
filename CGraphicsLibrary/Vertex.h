#pragma once

#include "Matrix.h"

/* for storing colour values between 0 and 1 */
struct Color_f {
    float r;
    float g;
    float b;

    Color_f();
    Color_f(float R, float G, float B);

    // Component-wise multiplication
    Color_f operator*(Color_f rhs) const;
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color();
    Color(unsigned char R, unsigned char G, unsigned char B);
    unsigned int packColor();
    Color_f convertToFloat();

    bool operator==(const Color& lhs) const;
};

struct Vertex
{
    vec4 pos;
    vec4 pos_WS;

    // Integer vec 3 containing rounded vertex coordinates
    vec3 pos_CS;

    vec4 normal;
    Color color;

    Vertex();
    Vertex(const vec4& Pos, const vec4& Pos_worldSpace, const vec4& Normal, Color Colour);
    void EnableCSCoords();
    void print();
};
