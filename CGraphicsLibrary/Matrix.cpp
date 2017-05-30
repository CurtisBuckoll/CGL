#include "Matrix.h"

#include <iostream>
#include <iomanip>
#include <math.h>

#include <random>

#define PI_OVER_180  0.0174532f


int getRandomEntry(int min, int max)
{
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(rng);
}

/*	mat4 --------------------------------------------------------------------------- */


mat4::mat4()
{
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			matrix[x][y] = 0.0;
		}
	}
	matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[3][3] = 1.0;
}


mat4::mat4(MAT_TYPE type)
{
    if (type == MAT_TYPE::PROJECTION)
    {
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                matrix[x][y] = 0.0;
            }
        }
        matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[2][3] = 1.0;
    }
}


void mat4::scale(double x, double y, double z)
{
	mat4 scale;
	scale.matrix[0][0] = x;
	scale.matrix[1][1] = y;
	scale.matrix[2][2] = z;
	*this = (*this) * scale;
}


void mat4::translate(double x, double y, double z)
{
	mat4 transform;
	transform.matrix[3][0] = x;
	transform.matrix[3][1] = y;
	transform.matrix[3][2] = z;
	*this = (*this) * transform;
}


void mat4::rotate(double angle, Axis axis)
{
	mat4 rotate;

	switch (axis)
	{
	case Axis::X :
		rotate.matrix[1][1] = cos(angle * PI_OVER_180);
		rotate.matrix[2][1] = sin(angle * PI_OVER_180);
		rotate.matrix[1][2] = -sin(angle * PI_OVER_180);
		rotate.matrix[2][2] = cos(angle * PI_OVER_180);
		break;

	case Axis::Y :
		rotate.matrix[0][0] = cos(angle * PI_OVER_180);
		rotate.matrix[2][0] = -sin(angle * PI_OVER_180);
		rotate.matrix[0][2] = sin(angle * PI_OVER_180);
		rotate.matrix[2][2] = cos(angle * PI_OVER_180);
		break;

	case Axis::Z :
		rotate.matrix[0][0] = cos(angle * PI_OVER_180);
		rotate.matrix[1][0] = sin(angle * PI_OVER_180);
		rotate.matrix[0][1] = -sin(angle * PI_OVER_180);
		rotate.matrix[1][1] = cos(angle * PI_OVER_180);
		break;
	}

	*this = (*this) * rotate;
}


void mat4::random(int min, int max)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            matrix[x][y] = (double)getRandomEntry(min, max);
        }
    }
}


void swapRows(mat4* curr, int r1, int r2)
{
    double temp;
    for (int x = 0; x < 4; x++)
    {
        temp = curr->matrix[x][r1];
        curr->matrix[x][r1] = curr->matrix[x][r2];
        curr->matrix[x][r2] = temp;
    }
}


/* Computes and returns a copy of the inverse of the current matrix
 * Pre: matrix must have a non-zero determinant
 */
mat4 mat4::inverse()
{
    mat4 curr = *this;
    mat4 res = mat4();

    double c;
    for (int i = 0; i < 4; i++)
    {
        for (int y = i+1; y < 4; y++)
        {
            if (curr.matrix[i][i] == 0)
            {
                // swap row
                for (int k = i + 1; k < 4; k++)
                {
                    if (curr.matrix[i][k] != 0)
                    {
                        swapRows(&curr, k, i);
                        swapRows(&res, k, i);
                    }
                }
            }

            c = curr.matrix[i][y] / curr.matrix[i][i];

            for (int x = i; x < 4; x++)
            {
                curr.matrix[x][y] = curr.matrix[x][y] - c * curr.matrix[x][i];
            }

            for (int x = 0; x < 4; x++)
            {
                res.matrix[x][y] = res.matrix[x][y] - c * res.matrix[x][i];
            }
        }
    }

    // back substitute
    for (int i = 3; i > 0; i--)
    {
        for (int y = i-1; y >= 0; y--)
        {
            c = curr.matrix[i][y] / curr.matrix[i][i];

            for (int x = i; x < 4; x++)
            {
                curr.matrix[x][y] = curr.matrix[x][y] - c * curr.matrix[x][i];
            }
            for (int x = 0; x < 4; x++)
            {
                res.matrix[x][y] = res.matrix[x][y] - c * res.matrix[x][i];
            }
        }
    }

    // reduce diagonal to 1's
    for (int y = 0; y < 4; y++)
    {
        c = 1 / curr.matrix[y][y];
        curr.matrix[y][y] = c * curr.matrix[y][y];

        for (int x = 0; x < 4; x++)
        {
            res.matrix[x][y] = c * res.matrix[x][y];
        }
    }

    return res;
}


void mat4::print()
{
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			std::cout  << std::setprecision(4) << matrix[x][y] << "\t";
		}
		std::cout << std::endl;
	}
}


mat4 mat4::operator*(const mat4& mat)
{
	mat4 result;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			double sum = 0.0;
			for (int k = 0; k < 4; k++)
			{
				sum += this->matrix[k][y] * mat.matrix[x][k];
			}
			result.matrix[x][y] = sum;
		}
	}

	return result;
}


vec4 mat4::operator*(const vec4& vec) const
{
	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < 4; i++)
	{
		result.x += matrix[i][0] * vec[i];
		result.y += matrix[i][1] * vec[i];
		result.z += matrix[i][2] * vec[i];
		result.w += matrix[i][3] * vec[i];
	}

    result.color = vec.color;
	return result;
}


/*	vec3 --------------------------------------------------------------------------- */


vec3::vec3() : color(0xffffffff)
{
	x = y = z = 0;
}
	

vec3::vec3(int x, int y, int z) : color(0xffffffff)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


vec3::vec3(int x, int y, int z, unsigned int color)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->color = color;
}


void vec3::print() 
{
	std::cout << "x: " << x << " y: " << y << " z: " << z << " colour: " << color << std::endl;
}


int vec3::operator[](int i) const
{
	switch (i)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		return 0;
	}
}


vec3 vec3::operator-(const vec3& p2) const
{
	return vec3(x - p2.x, y - p2.y, z - p2.z);
}


vec3& vec3::operator=(const vec4& rhs)
{
	this->x = (int)round(rhs.x);
	this->y = (int)round(rhs.y);
	this->z = (int)round(rhs.z);
    this->color = rhs.color;
	return *this;
}


/*	vec4 -------------------------------------------------------------------------- */


vec4::vec4()
{
	x = y = z = 0.0;
	w = 1.0;
}


vec4::vec4(double x, double y, double z, double w /*= 1*/)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


void vec4::normalize()
{
    double length = (double)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
    if ( length != 0)
    {
        x = x / length;
        y = y / length;
        z = z / length;
        w = w / length;
    }
}


void vec4::hgDivision()
{
    if ( w != 0)
    {
        x = x / w;
        y = y / w;
        z = z / w;
        w = 1.0;
    }
}


double vec4::dot(const vec4& rhs) const
{
    return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
}


/* Computes cross product between the difference of the three vertices passed in:
 * Performs (V2 - V1) x (V3 - V1)
 */
vec4 vec4::cross(const vec4& V2) const
{
    return vec4(y * V2.z - z * V2.y, z * V2.x - x * V2.z, x * V2.y - y * V2.x, 0.0);
}

// Returns the EUCLIDEAN magnitude of a vector - ignores the w component
double vec4::length()
{
    return (double)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}


void vec4::print()
{
	std::cout << "x: " << x << " y: " << y << " z: " << z << " w: " << w << std::endl;
}


double vec4::operator[](int i) const
{
	switch (i)
	{
	case 0 :
		return x;
		break;
	case 1 :
		return y;
		break;
	case 2 :
		return z;
		break;
	case 3 :
		return w;
		break;
	default:
		return 0.0;
	}
}


vec4 vec4::operator*(double scalar) const
{
    vec4 result = vec4(x * scalar, y * scalar, z * scalar, w);
    return result;
}

vec4 vec4::operator*(const mat4& rhs) const
{
    vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = 0; i < 4; i++)
    {
        result.x += (*this)[i] * rhs.matrix[0][i];
        result.y += (*this)[i] * rhs.matrix[1][i];
        result.z += (*this)[i] * rhs.matrix[2][i];
        result.w += (*this)[i] * rhs.matrix[3][i];
    }

    result.color = (*this).color;
    return result;
}

vec4 vec4::operator-(const vec4& p2) const
{
    vec4 result = vec4(x - p2.x, y - p2.y, z - p2.z, w - p2.w);
    return result;
}

vec4 vec4::operator+(const vec4& p2) const
{
    vec4 result = vec4(x + p2.x, y + p2.y, z + p2.z, w + p2.w);
    return result;
}

bool vec4::operator==(const vec4& p2) const
{
    return (x == p2.x) && (y == p2.y) && (z == p2.z) && (w == p2.w);
}

bool vec4::operator!=(const vec4& p2) const
{
    return !((x == p2.x) && (y == p2.y) && (z == p2.z) && (w == p2.w));
}
