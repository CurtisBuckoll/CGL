#include "Camera.h"

#include <SDL\SDL.h>


Camera::Camera()
{
	_CAMERA = mat4();
	_forward = vec4(0.0, 0.0, 1.0, 0.0);
	_right = vec4(1.0, 0.0, 0.0, 0.0);
	_moveAmount = 0.1;
	_rotateAmount = 1.0;
}


Camera::~Camera()
{
	// Empty
}


void Camera::updateCamera(bool* keys, Lighting* lightEngine, double deltaTime)
{
	double rotate = _rotateAmount * deltaTime;
	double move = _moveAmount * deltaTime;

	if (keys[SDLK_COMMA])
	{
		mat4 updateMatrix = mat4();
		updateMatrix.rotate(rotate, Axis::Y);
		_CAMERA = _CAMERA * updateMatrix;
		mat4 uMatrix_inv = updateMatrix.inverse();
		_forward = _forward * uMatrix_inv;
		_right = _right * uMatrix_inv;
		_forward.normalize();
		_right.normalize();
	}
	if (keys[SDLK_PERIOD])
	{
		mat4 updateMatrix = mat4();
		updateMatrix.rotate(-rotate, Axis::Y);
		_CAMERA = _CAMERA * updateMatrix;
		mat4 uMatrix_inv = updateMatrix.inverse();
		_forward = _forward * uMatrix_inv;
		_right = _right * uMatrix_inv;
		_forward.normalize();
		_right.normalize();
	}
	if (keys[SDLK_a])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _right * -move;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_d])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _right * move;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_w])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _forward * move;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_s])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _forward * -move;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}

	vec4 eyePoint = vec4(0.0, 0.0, 0.0, 1.0);
	eyePoint = _CAMERA * eyePoint;
	lightEngine->setEyePoint(eyePoint);
}


mat4 Camera::getCameraMatrix()
{
	return _CAMERA.inverse();
}