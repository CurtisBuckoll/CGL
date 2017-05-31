#pragma once

#include <vector>

#include "Line.h"
#include "DynamicArray.h"
#include "Matrix.h"
#include "zBuffer.h"
#include "Vertex.h"
#include "Lighting.h"
#include "Window.h"
#include "Texture2D.h"



class Polygon
{
public:
	/* Draws a filled convex polygon by connecting the list of points and using bi-linear interpolation
	 * Pre: Points passed in are specified in counter-clockwise order.
	 *		Points list cannot have more than two topmost points with common Y values.
	 *		Opacity expects a value between 0.0 and 1.0.
	 *		Vertices should have a specified colour.
	 */
    static void drawPolygonLERP(std::vector<Vertex>& points,
								double opacity, 
								bool wireframe, 
								zBuffer* zbuffer,
                                Window* window,
                                Lighting* lightEngine,
								Texture2D& texture);
};

