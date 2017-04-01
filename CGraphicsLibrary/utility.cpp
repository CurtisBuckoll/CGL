#include "utility.h"

/* Computes cross product between the difference of the three vertices passed in:
 * Performs (V2 - V1) x (V3 - V1)
 */
vec4 cross(const vec4& V1, const vec4& V2, const vec4& V3)
{
    vec4 b = V2 - V1;
    vec4 c = V3 - V1;
    vec4 result(0.0f, 0.0f, 0.0f, 0.0f);

    result.x = b.y * c.z - b.z * c.y;
    result.y = b.z * c.x - b.x * c.z;
    result.z = b.x * c.y - b.y * c.x;

    return result;
}


unsigned int getRandomColor()
{
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<int> distribution(0, 255);

    unsigned int a = 0xff;
    unsigned int r = distribution(rng);
    unsigned int g = distribution(rng);
    unsigned int b = distribution(rng);
    unsigned int color = ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

    return color;
}


int getRandomNumber(int min, int max)
{
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(rng);
}


unsigned int packColor(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xff << 24) + (r << 16) + (g << 8) + b;
}


unsigned int packColor(Color color)
{
    return (0xff << 24) + (color.r << 16) + (color.g << 8) + color.b;
}


void unpackColor(unsigned int color, unsigned char* r, unsigned char* g, unsigned char* b)
{
    *r = (color >> 16) & 0xff;
    *g = (color >> 8) & 0xff;
    *b = color & 0xff;
}


Color unpackColor(unsigned int color)
{
    return Color(((color >> 16) & 0xff), ((color >> 8) & 0xff), (color & 0xff));
}

// Multiply color values COMPONENT wise
Color multiplyColors(Color c1, Color c2)
{
    return Color((unsigned char)(c1.r * c2.r / 255.0f),
                 (unsigned char)(c1.g * c2.g / 255.0f),
                 (unsigned char)(c1.b * c2.b / 255.0f));
}

// Multiply color values COMPONENT wise - with one floating point
Color multiplyColors(Color c1, Color_f c2)
{
    return Color((unsigned char)(c1.r * c2.r),
                 (unsigned char)(c1.g * c2.g),
                 (unsigned char)(c1.b * c2.b));
}

Color computeDepthShading(float Z, Color objColor, zBuffer* zBuff)
{
    if (Z <= zBuff->depthNear) { return objColor; }
    if (Z >= zBuff->depthFar) { return zBuff->depthColor; }

    float ratio = (Z - zBuff->depthNear) / (zBuff->depthFar - zBuff->depthNear);

    unsigned char r = (unsigned char)(((1.0f - ratio) * objColor.r) + ((ratio) * zBuff->depthColor.r));
    unsigned char g = (unsigned char)(((1.0f - ratio) * objColor.g) + ((ratio) * zBuff->depthColor.g));
    unsigned char b = (unsigned char)(((1.0f - ratio) * objColor.b) + ((ratio) * zBuff->depthColor.b));

    return Color(r, g, b);
}
