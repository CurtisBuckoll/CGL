#pragma once

#include <random>
#include "zBuffer.h"
#include "Vertex.h"


vec4 cross(const vec4& V1, const vec4& V2, const vec4& V3);

unsigned int getRandomColor();

int getRandomNumber(int min, int max);

unsigned int packColor(unsigned char r, unsigned char g, unsigned char b);
unsigned int packColor(Color color);

void unpackColor(unsigned int color, unsigned char* r, unsigned char* g, unsigned char* b);
Color unpackColor(unsigned int color);

// Multiply color values COMPONENT wise
Color multiplyColors(Color c1, Color c2);

// Multiply color values COMPONENT wise - with one doubleing point
Color multiplyColors(Color c1, Color_f c2);

Color computeDepthShading(double Z, Color objColor, zBuffer* zBuff);
