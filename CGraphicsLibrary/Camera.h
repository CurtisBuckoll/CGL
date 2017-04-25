#pragma once

#include "Matrix.h"
#include "lighting.h"

class Camera
{
private:
	mat4 _CAMERA;
	vec4 _forward;
	vec4 _right;

public:
	Camera();
	~Camera();

	void updateCamera(bool* keys, Lighting* lightEngine);

	mat4 getCameraMatrix();
};

