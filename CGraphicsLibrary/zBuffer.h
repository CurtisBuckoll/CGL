#pragma once

#include "Vertex.h"

class zBuffer
{
public:
	double** buffer;
	int width;
	int height;

    // For atmospheric perspective
    Color depthColor;
    double depthNear;
    double depthFar;

	zBuffer();
	zBuffer(int w, int h, double depth_near, double depth_far, Color depth_color);
	~zBuffer();

	void reset();

	zBuffer& operator=(const zBuffer& arr);
};

