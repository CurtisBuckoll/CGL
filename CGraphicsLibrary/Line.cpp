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


bool cullLine(vec4 p0, vec4 p1, zBuffer* zbuffer)
{
    int w = zbuffer->width - 1;
    int h = zbuffer->height - 1;

    if ((p0.x < 0 && p1.x < 0) || (p0.x > w && p1.x > w) ||
        (p0.y < 0 && p1.y < 0) || (p0.y > h && p1.y > h) ||
        (p0.z < zbuffer->hither && p1.z < zbuffer->hither) || (p0.z > zbuffer->yon && p1.z > zbuffer->yon))
    {
        return true;
    }
    return false;
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
unsigned int getPixelWithOpacity(unsigned int color, unsigned int pixelColor, float opacity)
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


/* Antialiasing helper functions --------------------------------------------------- */

/*
unsigned int getPixelColorAntialiased(unsigned int oldColor, unsigned int color, float coverage)
{
    unsigned int r_old = (oldColor >> 16) & 0xff;
    unsigned int g_old = (oldColor >> 8) & 0xff;
    unsigned int b_old = oldColor & 0xff;

    unsigned int r = (color >> 16) & 0xff;
    unsigned int g = (color >> 8) & 0xff;
    unsigned int b = color & 0xff;

    r = (unsigned int)((r * coverage) + (r_old * (1 - coverage)));
    g = (unsigned int)((g * coverage) + (g_old * (1 - coverage)));
    b = (unsigned int)((b * coverage) + (b_old * (1 - coverage)));

    unsigned int newColor = (0xff << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    return newColor;
}
*/

/** Antialiasing - Octant dependent drawing functions:
 *  To be returned as function pointers by setDrawFunctionAA(): drawLineAA_Oct1 - drawLineAA_Oct8
 */
/*
void drawLineAA_Oct1(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel(p0.x + x, (int)round(p0.y + y), color);
}
void drawLineAA_Oct2(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel((int)round(p0.x + y), p0.y + x, color);
}
void drawLineAA_Oct3(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel((int)round(p0.x + -y), p0.y + x, color);
}
void drawLineAA_Oct4(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel(p0.x + -x, (int)round(p0.y + y), color);
}
void drawLineAA_Oct5(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel(p0.x + -x, (int)round(p0.y + -y), color);
}
void drawLineAA_Oct6(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel((int)round(p0.x + -y), p0.y + -x, color);
}
void drawLineAA_Oct7(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel((int)round(p0.x + y), p0.y + -x, color);
}
void drawLineAA_Oct8(vec3 p0, int x, float y, unsigned int color, Drawable* drawable)
{
    drawable->setPixel(p0.x + x, (int)round(p0.y + -y), color);
}


// Function pointer for Antialiased getPixel() functions
typedef void(*voidDrawFnAA)(vec3 p0, int x, float y, unsigned int color, Drawable* drawable);
*/

/** Antialiaised getPixel() functions
 *  To be returned as function pointers by getPixelFunction(): drawLineDDA_Oct1 - drawLineDDA_Oct4
 */
/*
unsigned int getPixelAA_Oct1(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel(p0.x + x, (int)round(p0.y + y));
}
unsigned int getPixelAA_Oct2(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel((int)round(p0.x + y), p0.y + x);
}
unsigned int getPixelAA_Oct3(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel((int)round(p0.x + -y), p0.y + x);
}
unsigned int getPixelAA_Oct4(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel(p0.x + -x, (int)round(p0.y + y));
}
unsigned int getPixelAA_Oct5(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel(p0.x + -x, (int)round(p0.y + -y));
}
unsigned int getPixelAA_Oct6(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel((int)round(p0.x + -y), p0.y + -x);
}
unsigned int getPixelAA_Oct7(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel((int)round(p0.x + y), p0.y + -x);
}
unsigned int getPixelAA_Oct8(vec3 p0, int x, float y, Drawable* drawable)
{
    return drawable->getPixel(p0.x + x, (int)round(p0.y + -y));
}


// Function pointer for Antialiased getPixel() functions
typedef unsigned int(*voidGetPixelFnAA)(vec3 p0, int x, float y, Drawable* drawable);


// Transform coordinates to octant I and return the corresponding AA line draw function
voidDrawFnAA getDrawFunctionAA(vec3* v, Octant octant)
{
    switch (octant)
    {
    case Octant::ONE:
        return drawLineAA_Oct1;

    case Octant::TWO:
        swapXY(v);
        return drawLineAA_Oct2;

    case Octant::THREE:
        v->x = -v->x;
        swapXY(v);
        return drawLineAA_Oct3;

    case Octant::FOUR:
        v->x = -v->x;
        return drawLineAA_Oct4;

    case Octant::FIVE:
        v->x = -v->x;
        v->y = -v->y;
        return drawLineAA_Oct5;

    case Octant::SIX:
        v->y = -v->y;
        v->x = -v->x;
        swapXY(v);
        return drawLineAA_Oct6;

    case Octant::SEVEN:
        v->y = -v->y;
        swapXY(v);
        return drawLineAA_Oct7;

    case Octant::EIGHT:
        v->y = -v->y;
        return drawLineAA_Oct8;

    default:
        return nullptr;
    }
}
*/

/**	Transform coordinates to octant I and return the corresponding DDA line draw function
 *	Transformations to vector v to be done by getDrawFunction(), which must be called first.
 */
/*
voidGetPixelFnAA getPixelFunction(Octant octant)
{
    switch (octant)
    {
    case Octant::ONE:
        return getPixelAA_Oct1;

    case Octant::TWO:
        return getPixelAA_Oct2;

    case Octant::THREE:
        return getPixelAA_Oct3;

    case Octant::FOUR:
        return getPixelAA_Oct4;

    case Octant::FIVE:
        return getPixelAA_Oct5;

    case Octant::SIX:
        return getPixelAA_Oct6;

    case Octant::SEVEN:
        return getPixelAA_Oct7;

    case Octant::EIGHT:
        return getPixelAA_Oct8;

    default:
        return nullptr;
    }
}
*/

/* Returns octant ONE through EIGHT that vector v resides in:
* Use for antialiasing
*/
/*
Octant getOctant_ALL(vec3* v)
{
    int octant = 0;

    if (v->y <= 0)
    {
        octant |= 0x4;
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
*/

/* End Antialiasing helper functions ------------------------------------------------- */

/*
void Line::DDA(vec3 p0, vec3 p1, unsigned int color, Drawable* panel)
{
    vec3 v = p1 - p0;
    Octant octant = getOctant(&p0, &p1, &v);
    voidDrawFn lineFunction = getDrawFunction(&v, octant);

    int dx = v.x;
    int dy = v.y;
    float slope = (float)dy / dx;		/// may have to deal with -ve denominator

    float y = 0.0f;
    for (int x = 0; x <= v.x; x++)
    {
        lineFunction(p0, x, (int)round(y), color, panel);
        y += slope;
    }
}
*/
/*
// Overloaded DDA that renders with an opacity
void Line::DDA(vec3 p0, vec3 p1, unsigned int color, float opacity, Drawable* panel)
{
    vec3 v = p1 - p0;
    Octant octant = getOctant(&p0, &p1, &v);
    voidDrawFnAA lineFunction = getDrawFunctionAA(&v, octant);
    voidGetPixelFnAA pixelFunction = getPixelFunction(octant);

    int dx = v.x;
    int dy = v.y;
    float slope = (float)dy / dx;

    float y = 0.0f;
    for (int x = 0; x <= v.x; x++)
    {
        if (opacity == 1.0f)
        {
            lineFunction(p0, x, y, color, panel);
        }
        else
        {
            unsigned int pixelColor = pixelFunction(p0, x, round(y), panel);
            unsigned int colorOpacity = getPixelWithOpacity(color, pixelColor, opacity);
            lineFunction(p0, x, y, colorOpacity, panel);
        }
        y += slope;
    }
}
*/

// Overloaded DDA that performs perspective correct linear interpolation
void Line::DDA(Vertex V0, Vertex V1, zBuffer* zbuffer, Window* window)
{
    if (cullLine(V0.pos, V1.pos, zbuffer))
    {
        return;
    }

    // Obtain rounded vertices
    vec3 p0; p0 = V0.pos;
    vec3 p1; p1 = V1.pos;
    vec3 v = p1 - p0;

    Octant octant = getOctant(&p0, &p1, &V0, &V1, &v);
    voidDrawFn lineFunction = getDrawFunction(&v, octant);

    int dx = v.x;
    int dy = v.y;
    float slope = (float)dy / dx;
    float dzdx = (float)(p1.z - p0.z) / dx;
    float zCurrent = (float)p0.z;

    float r_P0 = (float)V0.color.r;
    float g_P0 = (float)V0.color.g;
    float b_P0 = (float)V0.color.b;
    float r_P1 = (float)V1.color.r;
    float g_P1 = (float)V1.color.g;
    float b_P1 = (float)V1.color.b;


    // Perspective correct
    float z0_prime = 1.0f;
    float z1_prime = 1.0f;
    if (p0.z >= 1)
    {
        r_P0 = r_P0 / p0.z;
        g_P0 = g_P0 / p0.z;
        b_P0 = b_P0 / p0.z;
        z0_prime =  1.0f / p0.z;
    }
    if (p1.z >= 1)
    {
        r_P1 = r_P1 / p1.z;
        g_P1 = g_P1 / p1.z;
        b_P1 = b_P1 / p1.z;
        z1_prime = 1.0f / p1.z;
    }
    float dzPrime_dx = (z1_prime - z0_prime) / dx;

    float drdx = (r_P1 - r_P0) / dx;
    float dgdx = (g_P1 - g_P0) / dx;
    float dbdx = (b_P1 - b_P0) / dx;

    float r = (float)r_P0;
    float g = (float)g_P0;
    float b = (float)b_P0;

    float Y = 0.0f;
    for (int x = 0; x <= v.x; x++)
    {
        int y = (int)round(Y);

        int actualX = x;
        int actualY = y;

        transformCoord(p0, &actualX, &actualY, octant);

        if (actualX >= 0 && actualX <= zbuffer->width - 1 &&
            actualY >= 0 && actualY <= zbuffer->height - 1 &&
            zCurrent >= zbuffer->hither && zCurrent < zbuffer->buffer[actualX][actualY])
        {
            Color color(((unsigned char)round(r / z0_prime) & 0xff),
                        ((unsigned char)round(g / z0_prime) & 0xff),
                        ((unsigned char)round(b / z0_prime) & 0xff));
            color = computeDepthShading(zCurrent, color, zbuffer);
            //unsigned int pixelColor = packColor(color);

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

/* THIS IS OLD CODE OF ABOVE FUNCTION DELETE LATER
void Line::DDA(Vertex p0, Vertex p1, zBuffer* zbuffer, Drawable* panel)
{
    if (cullLine(p0.pos, p1.pos, zbuffer))
    {
        return;
    }

    vec4 v = p1.pos - p0.pos;
    Octant octant = getOctant(&p0, &p1, &v);
    voidDrawFn lineFunction = getDrawFunction(&v, octant);

    float dx = v.x;
    float dy = v.y;
    float slope = (float)dy / dx;
    float dzdx = (float)(p1.pos.z - p0.pos.z) / dx;
    float zCurrent = (float)p0.pos.z;

    float r_P0 = (float)p0.color.r;
    float g_P0 = (float)p0.color.g;
    float b_P0 = (float)p0.color.b;
    float r_P1 = (float)p1.color.r;
    float g_P1 = (float)p1.color.g;
    float b_P1 = (float)p1.color.b;

    // Perspective correct
    float z0_prime = 1.0f;
    float z1_prime = 1.0f;
    if (p0.pos.z >= 1)
    {
        r_P0 = r_P0 / p0.pos.z;
        g_P0 = g_P0 / p0.pos.z;
        b_P0 = b_P0 / p0.pos.z;
        z0_prime =  1.0f / p0.pos.z;
    }
    if (p1.pos.z >= 1)
    {
        r_P1 = r_P1 / p1.pos.z;
        g_P1 = g_P1 / p1.pos.z;
        b_P1 = b_P1 / p1.pos.z;
        z1_prime = 1.0f / p1.pos.z;
    }
    float dzPrime_dx = (z1_prime - z0_prime) / dx;

    float drdx = (r_P1 - r_P0) / dx;
    float dgdx = (g_P1 - g_P0) / dx;
    float dbdx = (b_P1 - b_P0) / dx;

    float r = r_P0;
    float g = g_P0;
    float b = b_P0;

    float Y = 0.0f;
    int xBound = (int)round(v.x);
    for (int x = 0; x <= xBound; x++)
    {
        int y = (int)round(Y);

        int actualX = x;
        int actualY = y;

        transformCoord(p0.pos, &actualX, &actualY, octant);

        if (actualX >= 0 && actualX <= zbuffer->width - 1 &&
            actualY >= 0 && actualY <= zbuffer->height - 1 &&
            zCurrent >= zbuffer->hither && zCurrent < zbuffer->buffer[actualX][actualY])
        {
            Color color(((unsigned char)round(r / z0_prime) & 0xff),
                        ((unsigned char)round(g / z0_prime) & 0xff),
                        ((unsigned char)round(b / z0_prime) & 0xff));
            color = computeDepthShading(zCurrent, color, zbuffer);
            unsigned int pixelColor = packColor(color);

            lineFunction(p0.pos, x, y, pixelColor, panel);
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
*/



/*
void Line::Bresenham(vec3 p0, vec3 p1, unsigned int color, Drawable* panel)
{
    vec3 v = p1 - p0;
    Octant octant = getOctant(&p0, &p1, &v);
    voidDrawFn lineFunction = getDrawFunction(&v, octant);

    int dx = v.x;
    int two_dx = dx << 1;
    int two_dy = v.y << 1;
    int err = two_dy - dx;
    int t2 = two_dy - two_dx;
    int y = 0;

    panel->setPixel(p0.x, (p0.y), color);

    for (int x = 1; x <= v.x; x++)
    {
        if (err >= 0)
        {
            err = err + t2;
            y = y + 1;
        }
        else
        {
            err = err + two_dy;
        }
        lineFunction(p0, x, y, color, panel);
    }
}
*/
/*
void Line::Antialiased(vec3 p0, vec3 p1, unsigned int color, Drawable* panel)
{
    vec3 v = p1 - p0;
    Octant octant = getOctant_ALL(&v);
    voidDrawFnAA lineFunction = getDrawFunctionAA(&v, octant);
    voidGetPixelFnAA pixelFunction = getPixelFunction(octant);

    int dx = v.x;
    int dy = v.y;
    float slope = (float)dy / dx;
    float coverage;
    unsigned int oldColor;
    unsigned int newColor;

    float y = 0;
    for (int x = 0; x <= v.x; x++)
    {
        for (int i = -1; i <= 1; i++)
        {
            int currY = p0.y + (int)round(y) + i;
            float v = (float)currY - (p0.y + y);
            float D = (float)abs((dx * v) / sqrt(dx*dx + dy*dy));
            D = D - ((float)LINEWIDTH / 2);
            if (D > R)
            {
                continue;
            }
            if (D < 0)
            {
                D = abs(D);
                float D_top = D;
                float D_bot = LINEWIDTH - D;
                coverage = (float)(1 - ((acos(D_top / R) - ((D_top * sqrt(RSQUARED - D_top * D_top)) / RSQUARED)) * (1 / PI)));
                if (D_bot < R)
                {
                    coverage += (float)(1 - ((acos(D_bot / R) - ((D_bot * sqrt(RSQUARED - D_bot * D_bot)) / RSQUARED)) * (1 / PI)));
                    coverage -= 1;
                }
            }
            else
            {
                coverage = (float)((acos(D / R) - ((D * sqrt(RSQUARED - D*D)) / RSQUARED)) * (1 / PI));
            }
            oldColor = pixelFunction(p0, x, round(y) + i, panel);
            newColor = getPixelColorAntialiased(oldColor, color, coverage);
            lineFunction(p0, x, round(y) + i, newColor, panel);
        }
        y += slope;
    }
}
*/
