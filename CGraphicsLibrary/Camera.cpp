#include "Camera.h"

#include <SDL\SDL.h>


Camera::Camera()
{
	_CAMERA = mat4();
	_forward = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	_right = vec4(1.0f, 0.0f, 0.0f, 0.0f);
}


Camera::~Camera()
{
	// Empty
}


void Camera::updateCamera(bool* keys, Lighting* lightEngine)
{
	const float moveAmount = 0.8f;
	const float rotateAmount = 10.0f;

	if (keys[SDLK_COMMA])
	{
		mat4 updateMatrix = mat4();
		updateMatrix.rotate(rotateAmount, Axis::Y);
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
		updateMatrix.rotate(-rotateAmount, Axis::Y);
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
		vec4 dir = _right * -moveAmount;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_d])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _right * moveAmount;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_w])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _forward * moveAmount;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}
	if (keys[SDLK_s])
	{
		mat4 updateMatrix = mat4();
		vec4 dir = _forward * -moveAmount;
		updateMatrix.translate(dir.x, dir.y, dir.z);
		_CAMERA = updateMatrix * _CAMERA;
	}

	vec4 eyePoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	eyePoint = _CAMERA * eyePoint;
	lightEngine->setEyePoint(eyePoint);
}


mat4 Camera::getCameraMatrix()
{
	return _CAMERA.inverse();
}