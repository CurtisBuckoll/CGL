#pragma once

#include "Matrix.h"
#include "lighting.h"


class Camera
{
private:
	mat4 _CAMERA;
	vec4 _forward;
	vec4 _right;

	double _moveAmount;
	double _rotateAmount;

public:
	Camera();
	~Camera();

	void updateCamera(bool* keys, Lighting* lightEngine, double deltaTime);

	mat4 getCameraMatrix();
};

