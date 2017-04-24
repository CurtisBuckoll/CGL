#pragma once

#include "Vertex.h"

class zBuffer
{
public:
	float** buffer;
	int width;
	int height;

    // For atmospheric perspective
    Color depthColor;
    float depthNear;
    float depthFar;

	zBuffer();
	zBuffer(int w, int h, float depth_near, float depth_far, Color depth_color);
	~zBuffer();

	void reset();

	zBuffer& operator=(const zBuffer& arr);
};

