#pragma once

#include "Point.h"
#include "Matrix.h"
#include "zBuffer.h"
#include "Vertex.h"
#include "Window.h"


#include <iostream>


class Line
{
public:
	/*	Draw line using DDA algorithm
	 *	colour interpolated between colour fields of p0 and p1.
	 *	z-testing performed.
	 */
    static void DDA(Vertex p0, Vertex p1, zBuffer* buffer, Window* window);
};

