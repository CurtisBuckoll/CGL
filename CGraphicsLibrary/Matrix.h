#pragma once


struct mat4;

enum class Axis {
	X = 'X',
	Y = 'Y',
	Z = 'Z'
};

// 4 x 1 floating point vector
struct vec4 {
	float x;
	float y;
	float z;
	float w;
	unsigned int color;

	vec4();
	vec4(float x, float y, float z, float w = 1.0f);
    void normalize();
    void hgDivision();
    float dot(const vec4& rhs) const;
    /* Computes cross product between the difference of the three vertices passed in:
     * Performs V1 x V2
     */
    vec4 cross(const vec4& V2) const;
    float length();
	void print();

	float operator[](int i) const;
	vec4 operator*(float scalar) const;
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
	float matrix[4][4];

	// Init to identity
	mat4();

    // Init to projection matrix
    mat4(MAT_TYPE type);

	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	void rotate(float angle, Axis axis);

    void random(int min, int max);
    mat4 inverse();
	void print();

	mat4 operator*(const mat4& mat);
	vec4 operator*(const vec4& vec) const;
};
