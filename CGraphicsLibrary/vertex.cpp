#include "Vertex.h"
#include <math.h>
#include <iostream>


Color_f::Color_f()
{
    r = 0.0;
    g = 0.0;
    b = 0.0;
}


Color_f::Color_f(double R, double G, double B)
{
    r = R;
    g = G;
    b = B;
}


Color_f Color_f::operator*(Color_f rhs) const
{
    return Color_f(r * rhs.r, g * rhs.g, b * rhs.b);
}


Color_f Color_f::operator*(double scalar) const
{
	return Color_f(r * scalar, g * scalar, b * scalar);
}


Color_f Color_f::operator+(Color_f rhs) const
{
	return Color_f(r + rhs.r, g + rhs.g, b + rhs.b);
}


Color_f Color_f::operator-(Color_f rhs) const
{
	return Color_f(r - rhs.r, g - rhs.g, b - rhs.b);
}


Color::Color()
{
    r = 0;
    g = 0;
    b = 0;
}


Color::Color(unsigned char R, unsigned char G, unsigned char B)
{
    r = R;
    g = G;
    b = B;
}


Color::Color(Color_f color)
{
	r = (unsigned char)(255 * color.r);
	g = (unsigned char)(255 * color.g);
	b = (unsigned char)(255 * color.b);
}


unsigned int Color::packColor()
{
    return (unsigned int)((0xff << 24) + (r << 16) + (g << 8) + b);
}


Color_f Color::convertTodouble()
{
    return Color_f((double)r / 255, (double)g / 255, (double)b / 255);
}


bool Color::operator==(const Color& rhs) const
{
    if (r == rhs.r && g == rhs.g && b == rhs.b)
    {
        return true;
    }
    return false;
}


Color Color::operator*(double scalar) const
{
	return Color((unsigned char)(r * scalar), (unsigned char)(g * scalar), (unsigned char)(b * scalar));
}


Vertex::Vertex()
{
    pos = vec4(0.0, 0.0, 0.0, 0.0);
    pos_CS = vec4(0.0, 0.0, 0.0, 0.0);
    pos_WS = vec4(0.0, 0.0, 0.0, 0.0);
    normal = vec4(0.0, 0.0, 0.0, 0.0);
    color = Color(0, 0, 0);
	uv = UV();
}


Vertex::Vertex(const vec4& Pos, const vec4& Pos_worldSpace, const vec4& Normal, Color Colour, const UV& uvCoords)
{
    pos = Pos;
    pos_CS = Pos;
    pos_WS = Pos_worldSpace;
    normal = Normal;
    color = Colour;
	uv = uvCoords;
}


void Vertex::EnableCSCoords()
{
    pos_CS = pos;
}


void Vertex::print()
{
    std::cout << "x: " << pos_CS.x << " y: " << pos_CS.y << " z: " << pos_CS.z;
    std::cout << " r: " << (int)color.r << " g: " << (int)color.b << " b: " << (int)color.b << std::endl;
}
