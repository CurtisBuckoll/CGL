#pragma once

#include "Matrix.h"
#include "lighting.h"


class Camera
{
private:
	mat4 _CAMERA;
	vec4 _forward;
	vec4 _right;

	float _moveAmount;
	float _rotateAmount;

public:
	Camera();
	~Camera();

	void updateCamera(bool* keys, Lighting* lightEngine, float deltaTime);

	mat4 getCameraMatrix();
};

