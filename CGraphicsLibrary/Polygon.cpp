#include "Polygon.h"

#include "utility.h"

#include <iostream>
#include <math.h>


/* Polygon Helpers ----------------------------------------------------------------- */


/* Compute signed area of polygon
 * Sign of result determines vertex winding: +ve: CCW
 * A = 1/2 * (x1*y2 - x2*y1 + x2*y3 - x3*y2 + ... + xn*y1 - x1*yn)
 */
inline bool isCCW(const std::vector<Vertex>& vertices)
{
    int result = 0;
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        vec3 v1 = vertices[i].pos_CS;
        vec3 v2 = vertices[(i+1) % vertices.size()].pos_CS;
        result += (v1.x * v2.y) - (v2.x * v1.y);
    }
    return result > 0;
}


inline float getSlopeInverted(const DynamicArray<Vertex>& arr, int baseIndex)
{
    int dx = arr[baseIndex + 1].pos_CS.x - arr[baseIndex].pos_CS.x;
    int dy = arr[baseIndex + 1].pos_CS.y - arr[baseIndex].pos_CS.y;
    if (dy == 0)
    {
        return 0;
    }
    return -((float)dx / dy);
}


/*
inline float getSlope_dzdy(DynamicArray<Vertex> arr, int baseIndex)
{
    int dz = arr[baseIndex + 1].pos_CS.z - arr[baseIndex].pos_CS.z;
    int dy = arr[baseIndex + 1].pos_CS.y - arr[baseIndex].pos_CS.y;
    if (dy == 0)
    {
        return 0;
    }
    return -(float)dz / dy;
}
*/


// This should be used for (z') lerp
inline float getSlope_dzPrimedy(float v1z, float v2z, float dy)
{
	if (dy == 0)
	{
		return 0;
	}
	float dz = v2z - v1z;

	return -dz / dy;
}


inline vec4 getSlope_vertex(const vec4& v1, const vec4& v2, float dy)
{
    if (dy == 0)
    {
        return vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    vec4 dv = v2 - v1;

    return dv * (-1.0f / dy);
}


inline Color_f getSlope_color(const Color_f& c1, const Color_f& c2, float dy)
{
	if (dy == 0)
	{
		return Color_f(0.0f, 0.0f, 0.0f);
	}
	Color_f dc = c2 - c1;

	return dc * (-1.0f / dy);
}


/*  Performs perspective correct interpolation
 */
/*
inline Color interpolateColor(float currDist, float totalDist, Color P0, Color P1, float z_0, float z_1)
{
    if (totalDist == 0)
    {
        return P0;
    }

    float distFraction = (float)currDist / totalDist;

    unsigned char r;
    unsigned char g;
    unsigned char b;

    if (z_0 == 0 || z_1 == 0)
    {
        r = (unsigned char)((distFraction * P1.r) + ((1 - distFraction) * P0.r));
        g = (unsigned char)((distFraction * P1.g) + ((1 - distFraction) * P0.g));
        b = (unsigned char)((distFraction * P1.b) + ((1 - distFraction) * P0.b));
    }
    else
    {
        r = (unsigned char)(((1 - distFraction) * (P0.r / z_0) + distFraction * (P1.r / z_1)) /
                            ((1 - distFraction) * (1 / z_0) + distFraction * (1 / z_1)));
        g = (unsigned char)(((1 - distFraction) * (P0.g / z_0) + distFraction * (P1.g / z_1)) /
                            ((1 - distFraction) * (1 / z_0) + distFraction * (1 / z_1)));
        b = (unsigned char)(((1 - distFraction) * (P0.b / z_0) + distFraction * (P1.b / z_1)) /
                            ((1 - distFraction) * (1 / z_0) + distFraction * (1 / z_1)));
    }

    return Color(r, g, b);
}
*/


/* End helper functions ------------------------------------------------------------ */


/** Draws a filled convex polygon by connecting the list of points and using bi-linear interpolation
 *  Pre: Points passed in are specified in counter-clockwise order.
 *       Points list cannot have more than two topmost points with common Y values.
 *       Opacity expects a value between 0.0 and 1.0.
 *		 Vertices should have a specified colour.
 */
void Polygon::drawPolygonLERP(std::vector<Vertex>& points,
                              float opacity, bool wireframe,
                              zBuffer* zbuffer,
                              Window* window,
                              Lighting* lightEngine)
{
	// Draw wireframe if enabled
	if (wireframe)
	{
		for (unsigned int i = 0; i < points.size() - 1; i++)
		{
			Line::DDA(points[i], points[i + 1], zbuffer, window);
		}
		Line::DDA(points[0], points[points.size() - 1], zbuffer, window);
		return;
	}

	// Enable rounded integer vertex coordinates
	for (unsigned int i = 0; i < points.size(); i++)
	{
		points[i].EnableCSCoords();
	}

	// Find max/min y values
	int minY = points[0].pos_CS.y;
	int maxY = points[0].pos_CS.y;
	int indexMax = 0;
	int indexMin = 0;

	for (unsigned int i = 1; i < points.size(); i++)
	{
		if (points[i].pos_CS.y > maxY)
		{
			maxY = points[i].pos_CS.y;
			indexMax = i;
		}
		if (points[i].pos_CS.y < minY)
		{
			minY = points[i].pos_CS.y;
			indexMin = i;
		}
	}

	// Check for multiple max points with the same Y value
	int numMax = -1;
	int nextMax = 0;
	int maxL_offset = 0;
	int maxR_offset = 0;
	for (unsigned int i = 0; i < points.size(); i++)
	{
		if (maxY == points[i].pos_CS.y)
		{
			numMax++;
			nextMax = i;
		}
	}
	maxL_offset = numMax;
	if (nextMax - indexMax > 1)
	{
		maxR_offset = (int)(points.size() - 1) - indexMax;
		maxL_offset = 0;
	}

	// Add to left and right arrays
	DynamicArray<Vertex> left_array;
	DynamicArray<Vertex> right_array;

	int currentIndex = indexMax + maxL_offset;
	left_array.append(points[currentIndex % points.size()]);
	while (currentIndex != indexMin)
	{
		currentIndex = (currentIndex + 1) % points.size();
		left_array.append(points[currentIndex]);
	}
	currentIndex = indexMax + maxR_offset;
	right_array.append(points[currentIndex % points.size()]);
	while (currentIndex != indexMin)
	{
		currentIndex = (int)(currentIndex + points.size() - 1) % points.size();
		right_array.append(points[currentIndex]);
	}

	// Draw the scan lines
	int index_L = 0;
	int index_R = 0;
	float slope_L = getSlopeInverted(left_array, index_L);
	float slope_R = getSlopeInverted(right_array, index_R);
	float dy_L = (float)(left_array[index_L + 1].pos_CS.y - left_array[index_L].pos_CS.y);
	float dy_R = (float)(right_array[index_R + 1].pos_CS.y - right_array[index_R].pos_CS.y);

	/** Set up Z Coord interpolation: this is persp correct (z') **/
	float zCoord_L1 = 1.0f / left_array[index_L].pos.z;
	float zCoord_L2 = 1.0f / left_array[index_L + 1].pos.z;
	float zCoord_R1 = 1.0f / right_array[index_R].pos.z;
	float zCoord_R2 = 1.0f / right_array[index_R + 1].pos.z;
	float zPrimeSlope_L = getSlope_dzPrimedy(zCoord_L1, zCoord_L2, dy_L);
	float zPrimeSlope_R = getSlope_dzPrimedy(zCoord_R1, zCoord_R2, dy_R);

	/** Set up Perspective Correct vertex colour interpolation **/
	Color_f color_L1 = left_array[index_L].color.convertToFloat() * (1.0f / left_array[index_L].pos.z);
	Color_f color_L2 = left_array[index_L + 1].color.convertToFloat() * (1.0f / left_array[index_L + 1].pos.z);
	Color_f color_R1 = right_array[index_R].color.convertToFloat() * (1.0f / right_array[index_R].pos.z);
	Color_f color_R2 = right_array[index_R + 1].color.convertToFloat() * (1.0f / right_array[index_R + 1].pos.z);
	Color_f colorSlope_L = getSlope_color(color_L1, color_L2, dy_L);
	Color_f colorSlope_R = getSlope_color(color_R1, color_R2, dy_R);

	/** Set up Perspective Correct normal interpolation **/
	vec4 norm_L1 = left_array[index_L].normal * (1.0f / left_array[index_L].pos.z);
	vec4 norm_L2 = left_array[index_L + 1].normal * (1.0f / left_array[index_L + 1].pos.z);
	vec4 norm_R1 = right_array[index_R].normal * (1.0f / right_array[index_R].pos.z);
	vec4 norm_R2 = right_array[index_R + 1].normal * (1.0f / right_array[index_R + 1].pos.z);
	vec4 normSlope_L = getSlope_vertex(norm_L1, norm_L2, dy_L);
	vec4 normSlope_R = getSlope_vertex(norm_R1, norm_R2, dy_R);

	/** Set up Perspective Correct WSC interpolation **/
	vec4 wsc_L1 = left_array[index_L].pos_WS * (1.0f / left_array[index_L].pos.z);
	vec4 wsc_L2 = left_array[index_L + 1].pos_WS * (1.0f / left_array[index_L + 1].pos.z);
	vec4 wsc_R1 = right_array[index_R].pos_WS * (1.0f / right_array[index_R].pos.z);
	vec4 wsc_R2 = right_array[index_R + 1].pos_WS * (1.0f / right_array[index_R + 1].pos.z);
	vec4 wscSlope_L = getSlope_vertex(wsc_L1, wsc_L2, dy_L);
	vec4 wscSlope_R = getSlope_vertex(wsc_R1, wsc_R2, dy_R);

	for (int y = maxY; y > minY; y--)
	{
		if (y == left_array[index_L + 1].pos_CS.y)
		{
			index_L++;
			slope_L = getSlopeInverted(left_array, index_L);
			float dy = (float)(left_array[index_L + 1].pos_CS.y - left_array[index_L].pos_CS.y);

			zCoord_L1 = zCoord_L2;
			zCoord_L2 = 1.0f / left_array[index_L + 1].pos.z;
			zPrimeSlope_L = getSlope_dzPrimedy(zCoord_L1, zCoord_L2, dy);

			color_L1 = color_L2;
			color_L2 = left_array[index_L + 1].color.convertToFloat() * (1.0f / left_array[index_L + 1].pos.z);
			colorSlope_L = getSlope_color(color_L1, color_L2, dy);

			norm_L1 = norm_L2;
			norm_L2 = left_array[index_L + 1].normal * (1.0f / left_array[index_L + 1].pos.z);
			normSlope_L = getSlope_vertex(norm_L1, norm_L2, dy);

			wsc_L1 = wsc_L2;
			wsc_L2 = left_array[index_L + 1].pos_WS * (1.0f / left_array[index_L + 1].pos.z);
			wscSlope_L = getSlope_vertex(wsc_L1, wsc_L2, dy);
		}

		if (y == right_array[index_R + 1].pos_CS.y)
		{
			index_R++;
			slope_R = getSlopeInverted(right_array, index_R);
			float dy = (float)(right_array[index_R + 1].pos_CS.y - right_array[index_R].pos_CS.y);

			zCoord_R1 = zCoord_R2;
			zCoord_R2 = 1.0f / right_array[index_R + 1].pos.z;
			zPrimeSlope_R = getSlope_dzPrimedy(zCoord_R1, zCoord_R2, dy);

			color_R1 = color_R2;
			color_R2 = right_array[index_R + 1].color.convertToFloat() * (1.0f / right_array[index_R + 1].pos.z);
			colorSlope_R = getSlope_color(color_R1, color_R2, dy);

			norm_R1 = norm_R2;
			norm_R2 = right_array[index_R + 1].normal * (1.0f / right_array[index_R + 1].pos.z);
			normSlope_R = getSlope_vertex(norm_R1, norm_R2, dy);

			wsc_R1 = wsc_R2;
			wsc_R2 = right_array[index_R + 1].pos_WS * (1.0f / right_array[index_R + 1].pos.z);
			wscSlope_R = getSlope_vertex(wsc_R1, wsc_R2, dy);

		}

		int L_endpoint = (int)round((left_array[index_L].pos_CS.y - y) * slope_L + left_array[index_L].pos_CS.x);
		int R_endpoint = (int)round((right_array[index_R].pos_CS.y - y) * slope_R + right_array[index_R].pos_CS.x);

		float totalHorizontalDist = (float)abs(L_endpoint - R_endpoint);
		int currHorizontalDist = 0;

		int dx = (R_endpoint - L_endpoint);

		// Interpolation across scan lines
		float dzPrimedx;
		vec4 dNorm_dx;
		vec4 dWSC_dx;
		Color_f dcdx;

		if (dx != 0)
		{
			dzPrimedx = (zCoord_R1 - zCoord_L1) / dx;
			dNorm_dx = (norm_R1 - norm_L1) *  (1.0f / dx);
			dWSC_dx = (wsc_R1 - wsc_L1) *  (1.0f / dx);
			dcdx = (color_R1 - color_L1) * (1.0f / dx);
		}
		else
		{
			dzPrimedx = 0.0f;
			dNorm_dx = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			dWSC_dx = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			dcdx = Color_f(0.0f, 0.0f, 0.0f);
		}

		float zPrimeCurrent = zCoord_L1;
		vec4 normCurrent = norm_L1;
		vec4 wscCurrent = wsc_L1;
		Color_f colorCurrent = color_L1;

		for (int x = L_endpoint; x < R_endpoint; x++)
		{
			float PerspCorr_Z = 1.0f / zPrimeCurrent;
			vec4 PerspCorr_Normal = normCurrent * PerspCorr_Z;
			vec4 PerspCorr_WSC = wscCurrent * PerspCorr_Z;
			Color_f PerspCorr_Color = colorCurrent * PerspCorr_Z;

			if (x >= 0 && x < zbuffer->width && y >= 0 && y < zbuffer->height && PerspCorr_Z < zbuffer->buffer[x][y])
			{
				Color color = Color(PerspCorr_Color);

				//color = points[0].color;

				// Get pixel value for lighting and depth shading
				color = lightEngine->PerformLightingCalculation(color, PerspCorr_Normal, PerspCorr_WSC);
				color = computeDepthShading(PerspCorr_Z, color, zbuffer);

				window->setPixel(x, y, color);
				zbuffer->buffer[x][y] = PerspCorr_Z;
			}

			// Increment lerp values
			currHorizontalDist++;
			zPrimeCurrent += dzPrimedx;
			colorCurrent = colorCurrent + dcdx;
			normCurrent = normCurrent + dNorm_dx;
			wscCurrent = wscCurrent + dWSC_dx;
		}

		// Increment lerp values
		zCoord_L1 += zPrimeSlope_L;
		zCoord_R1 += zPrimeSlope_R;

		color_L1 = color_L1 + colorSlope_L;
		color_R1 = color_R1 + colorSlope_R;

		norm_L1 = norm_L1 + normSlope_L;
		norm_R1 = norm_R1 + normSlope_R;

		wsc_L1 = wsc_L1 + wscSlope_L;
		wsc_R1 = wsc_R1 + wscSlope_R;
	}
}
