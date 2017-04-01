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
	 *	colour not interpolated.
	 */
    //static void DDA(vec3 p0, vec3 p1, unsigned int color, Drawable* panel);

	/*	Draw line using DDA algorithm
	 *	opacity field applied to line.
	 *	colour not interpolated.
	 */
    //static void DDA(vec3 p0, vec3 p1, unsigned int color, float opacity, Drawable* panel);

	/*	Draw line using DDA algorithm
	 *	colour interpolated between colour fields of p0 and p1.
	 *	z-testing performed.
	 */
    static void DDA(Vertex p0, Vertex p1, zBuffer* buffer, Window* window);

    //static void Bresenham(vec3 p0, vec3 p1, unsigned int color, Drawable* panel);
    //static void Antialiased(vec3 p0, vec3 p1, unsigned int color, Drawable* panel);
};

