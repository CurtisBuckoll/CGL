#include "zBuffer.h"

#include <iostream>


zBuffer::zBuffer()
{
	width = 0;
	height = 0;
	buffer = nullptr;
}


zBuffer::zBuffer(int w, int h, float Hither, float Yon,
                 float xLow, float yLow,
                 float xHigh, float yHigh) :
	width(w),
	height(h),
    hither(Hither),
    yon(Yon),
    xLo(xLow),
    yLo(yLow),
    xHi(xHigh),
    yHi(yHigh)
{
	buffer = new float*[w];
	for (int x = 0; x < w; x++)
	{
		buffer[x] = new float[h];
		for (int y = 0; y < h; y++)
		{
            buffer[x][y] = Yon;
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
		buffer = new float*[width];
		for (int x = 0; x < width; x++)
		{
			buffer[x] = new float[height];
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
			buffer[x][y] = yon;
		}
	}
}