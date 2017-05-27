#include "zBuffer.h"

#include <iostream>
#include <limits>


zBuffer::zBuffer()
{
	width = 0;
	height = 0;
	buffer = nullptr;
}


zBuffer::zBuffer(int w, int h, double depth_near, double depth_far, Color depth_color) :
	width(w),
	height(h),
	depthNear(depth_near),
	depthFar(depth_far),
	depthColor(depth_color)
{
	buffer = new double*[w];
	for (int x = 0; x < w; x++)
	{
		buffer[x] = new double[h];
		for (int y = 0; y < h; y++)
		{
			buffer[x][y] = std::numeric_limits<double>::max();
		}
	}
}


zBuffer::~zBuffer()
{
	if (buffer == nullptr)
	{
		return;
	}
	for (int x = 0; x < width; x++)
	{
		if (buffer[x] != nullptr)
		{
			delete[] buffer[x];
		}
	}
	delete[] buffer;
}


zBuffer& zBuffer::operator=(const zBuffer& rhs)
{
	width = rhs.width;
	height = rhs.width;

	if (rhs.buffer == nullptr)
	{
		buffer = nullptr;
	}
	else
	{
		buffer = new double*[width];
		for (int x = 0; x < width; x++)
		{
			buffer[x] = new double[height];
			for (int y = 0; y < height; y++)
			{
				buffer[x][y] = rhs.buffer[x][y];
			}
		}
	}

	return *this;
}

void zBuffer::reset()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			buffer[x][y] = std::numeric_limits<double>::max();
		}
	}
}