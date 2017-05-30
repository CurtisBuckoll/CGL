#pragma once

#include "Matrix.h"

/* UV coordinates for texture mapping */
struct UV {
	double u;
	double v;

	UV();
	UV(double U, double V);

	UV operator*(double scalar) const;
	UV operator+(UV rhs) const;
	UV operator-(UV rhs) const;
};

/* for storing colour values between 0 and 1 */
struct Color_f {
    double r;
    double g;
    double b;

    Color_f();
    Color_f(double R, double G, double B);

    // Component-wise multiplication
    Color_f operator*(Color_f rhs) const;

	// Scalar multiplication
	Color_f operator*(double scalar) const;

	Color_f operator+(Color_f rhs) const;
	Color_f operator-(Color_f rhs) const;
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color();
    Color(unsigned char R, unsigned char G, unsigned char B);
	Color(Color_f color);

    unsigned int packColor();
    Color_f convertTodouble();

    bool operator==(const Color& rhs) const;

	// Scalar multiplication
	Color operator*(double scalar) const;
};

struct Vertex
{
    vec4 pos;
    vec4 pos_WS;

    // Integer vec 3 - contains rounded vertex coordinates
    vec3 pos_CS;

    vec4 normal;
    Color color;
	UV uv;

    Vertex();
    Vertex(const vec4& Pos, const vec4& Pos_worldSpace, const vec4& Normal, Color Colour, const UV& uvCoords = UV());
    void EnableCSCoords();
    void print();
};
