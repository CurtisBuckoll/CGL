#include "Line.h"

#include "utility.h"

#include <math.h>

// for antialiasing:
#define		R			0.7071		// radius
#define		RSQUARED	0.5			// radius^2
#define		PI			3.1415
#define		LINEWIDTH	1


/* DDA/Bresenham Line Drawing Helper Functions ------------------------------------- */


// Swap X, Y coordinates of a point
void swapXY(vec3* p)
{
    int temp = p->x;
    p->x = p->y;
    p->y = temp;
}


// Swap data contained in two points
void swapVertices(Vertex* p0, Vertex* p1)
{
    Vertex temp = *p0;
    *p0 = *p1;
    *p1 = temp;
}

// Swap data contained in vec3's
void swapPoints(vec3* p0, vec3* p1)
{
    vec3 temp = *p0;
    *p0 = *p1;
    *p1 = temp;
}


/** DDA/Bresenham - Octant dependent drawing functions:
 *  To be returned as function pointers by setDrawFunctionBres(): drawLine_Oct1 - drawLine_Oct8
 */
void drawLine_Oct1(const vec3& p0, int x, int y, Color color, Window* window)
{
	window->setPixel(p0.x + x, p0.y + y, color);
}
void drawLine_Oct2(const vec3& p0, int x, int y, Color color, Window* window)
{
	window->setPixel(p0.x + y, p0.y + x, color);
}
void drawLine_Oct3(const vec3& p0, int x, int y, Color color, Window* window)
{
	window->setPixel(p0.x + -y, p0.y + x, color);
}
void drawLine_Oct4(const vec3& p0, int x, int y, Color color, Window* window)
{
	window->setPixel(p0.x + -x, p0.y + y, color);
}


// Function pointer for DDA/Bresenham octant-dependent drawing
typedef void(*voidDrawFn)(const vec3& p0, int x, int y, Color color, Window* window);


// Transform coordinates to octant I and return the corresponding DDA/Bresenham line draw function
voidDrawFn getDrawFunction(vec3* v, Octant octant)
{
    switch (octant)
    {
    case Octant::ONE:
        return drawLine_Oct1;

    case Octant::TWO:
        swapXY(v);
        return drawLine_Oct2;

    case Octant::THREE:
        v->x = -v->x;
        swapXY(v);
        return drawLine_Oct3;

    case Octant::FOUR:
        v->x = -v->x;
        return drawLine_Oct4;

    default:
        return nullptr;
    }
}


/** Returns octant ONE through FOUR that vector v resides in:
 *	Converts cases FIVE through EIGHT to cases ONE through FOUR and reverses points
 */
Octant getOctant(vec3* p0, vec3* p1, Vertex* V0, Vertex* V1, vec3* v)
{
    int octant = 0;

    if (v->y <= 0)
    {
        swapPoints(p0, p1);
        swapVertices(V0, V1);
        *v = *p1 - *p0;
    }
    if (v->x <= 0)
    {
        octant |= 0x2;
    }
    if (abs(v->x) < abs(v->y))
    {
        octant |= 0x1;
    }

    return (Octant)octant;
}


// Blends desired colour with existing pixel's colour based on opacity
unsigned int getPixelWithOpacity(unsigned int color, unsigned int pixelColor, double opacity)
{
    unsigned int r_pixel = (pixelColor >> 16) & 0xff;
    unsigned int g_pixel = (pixelColor >> 8) & 0xff;
    unsigned int b_pixel = pixelColor & 0xff;

    unsigned int r = (color >> 16) & 0xff;
    unsigned int g = (color >> 8) & 0xff;
    unsigned int b = color & 0xff;

    r = (unsigned int)((r * opacity) + (r_pixel * (1 - opacity)));
    g = (unsigned int)((g * opacity) + (g_pixel * (1 - opacity)));
    b = (unsigned int)((b * opacity) + (b_pixel * (1 - opacity)));

    unsigned int newColor = (0xff << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    return newColor;
}

void transformCoord(vec3 p0, int* x, int* y, Octant octant)
{
    int X = *x;
    int Y = *y;

    switch (octant)
    {
    case (Octant::ONE) :
        *x = p0.x + X;
        *y = p0.y + Y;
        break;

    case (Octant::TWO) :
        *x = p0.x + Y;
        *y = p0.y + X;
        break;

    case (Octant::THREE) :
        *x = p0.x + -Y;
        *y = p0.y + X;
        break;

    case (Octant::FOUR) :
        *x = p0.x + -X;
        *y = p0.y + Y;
        break;
    }
}


// Overloaded DDA that performs perspective correct linear interpolation
void Line::DDA(Vertex V0, Vertex V1, zBuffer* zbuffer, Window* window)
{
    // Obtain rounded vertices
    vec3 p0; p0 = V0.pos;
    vec3 p1; p1 = V1.pos;
    vec3 v = p1 - p0;

    Octant octant = getOctant(&p0, &p1, &V0, &V1, &v);
    voidDrawFn lineFunction = getDrawFunction(&v, octant);

    int dx = v.x;
    int dy = v.y;
    double slope = (double)dy / dx;
    double dzdx = (double)(p1.z - p0.z) / dx;
    double zCurrent = (double)p0.z;

    double r_P0 = (double)V0.color.r;
    double g_P0 = (double)V0.color.g;
    double b_P0 = (double)V0.color.b;
    double r_P1 = (double)V1.color.r;
    double g_P1 = (double)V1.color.g;
    double b_P1 = (double)V1.color.b;


    // Perspective correct
    double z0_prime = 1.0;
    double z1_prime = 1.0;
    if (p0.z >= 1)
    {
        r_P0 = r_P0 / p0.z;
        g_P0 = g_P0 / p0.z;
        b_P0 = b_P0 / p0.z;
        z0_prime =  1.0 / p0.z;
    }
    if (p1.z >= 1)
    {
        r_P1 = r_P1 / p1.z;
        g_P1 = g_P1 / p1.z;
        b_P1 = b_P1 / p1.z;
        z1_prime = 1.0 / p1.z;
    }
    double dzPrime_dx = (z1_prime - z0_prime) / dx;

    double drdx = (r_P1 - r_P0) / dx;
    double dgdx = (g_P1 - g_P0) / dx;
    double dbdx = (b_P1 - b_P0) / dx;

    double r = (double)r_P0;
    double g = (double)g_P0;
    double b = (double)b_P0;

    double Y = 0.0;
    for (int x = 0; x <= v.x; x++)
    {
        int y = (int)round(Y);

        int actualX = x;
        int actualY = y;

        transformCoord(p0, &actualX, &actualY, octant);

        if (actualX >= 0 && actualX <= zbuffer->width - 1 &&
            actualY >= 0 && actualY <= zbuffer->height - 1 &&
            zCurrent < zbuffer->buffer[actualX][actualY])
        {
            Color color(((unsigned char)round(r / z0_prime) & 0xff),
                        ((unsigned char)round(g / z0_prime) & 0xff),
                        ((unsigned char)round(b / z0_prime) & 0xff));
            color = computeDepthShading(zCurrent, color, zbuffer);

            lineFunction(p0, x, y, color, window);
            zbuffer->buffer[actualX][actualY] = zCurrent;
        }

        r += drdx;
        g += dgdx;
        b += dbdx;
        Y += slope;
        z0_prime += dzPrime_dx;
        zCurrent += dzdx;
    }
}
