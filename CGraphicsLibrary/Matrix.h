#pragma once


struct mat4;

enum class Axis {
	X = 'X',
	Y = 'Y',
	Z = 'Z'
};

// 4 x 1 doubleing point vector
struct vec4 {
	double x;
	double y;
	double z;
	double w;
	unsigned int color;

	vec4();
	vec4(double x, double y, double z, double w = 1.0);
    void normalize();
    void hgDivision();
    double dot(const vec4& rhs) const;
    /* Computes cross product between the difference of the three vertices passed in:
     * Performs V1 x V2
     */
    vec4 cross(const vec4& V2) const;
    double length();
	void print();

	double operator[](int i) const;
	vec4 operator*(double scalar) const;
    vec4 operator*(const mat4& rhs) const;
    vec4 operator-(const vec4& p2) const;
    vec4 operator+(const vec4& p2) const;
    bool operator==(const vec4& p2) const;
    bool operator!=(const vec4& p2) const;
};

// 3 x 1 integer vector
struct vec3 {
	int x;
	int y;
	int z;
	unsigned int color;

	vec3();
	vec3(int x, int y, int z);
	vec3(int x, int y, int z, unsigned int color);
	void print();

	int operator[](int i) const;
	vec3 operator-(const vec3& p2) const;
	vec3& operator=(const vec4& rhs);
};

enum class MAT_TYPE
{
    PROJECTION = 0
};

// 4 x 4 matrix
struct mat4 {
	double matrix[4][4];

	// Init to identity
	mat4();

    // Init to projection matrix
    mat4(MAT_TYPE type);

	void scale(double x, double y, double z);
	void translate(double x, double y, double z);
	void rotate(double angle, Axis axis);

    void random(int min, int max);
    mat4 inverse();
	void print();

	mat4 operator*(const mat4& mat);
	vec4 operator*(const vec4& vec) const;
};
