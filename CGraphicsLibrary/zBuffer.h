#pragma once

#include "Vertex.h"

class zBuffer
{
public:
	float** buffer;
	int width;
	int height;

    float hither;
    float yon;
    float xLo;
    float xHi;
    float yLo;
    float yHi;

    // For atmospheric perspective
    Color depthColor;
    float depthNear;
    float depthFar;

	zBuffer();
    zBuffer(int w, int h, float Hither, float Yon,
            float xLow = 1.0f, float yLow = 1.0f,
            float xHigh = 1.0f, float yHigh = 1.0f);
	~zBuffer();

	void reset();

	zBuffer& operator=(const zBuffer& arr);
};

