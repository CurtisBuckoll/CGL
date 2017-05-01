#include "FrameRateLimiter.h"

#include <iostream>
#include <SDL\SDL.h>


FrameRateLimiter::FrameRateLimiter(unsigned int FPS, float velocity, unsigned int printFPSfreq)
{
	_FPS = FPS;
	_MSPF = 1000.0f / FPS;
	_velocity = velocity;
	_printFrequency = printFPSfreq;
}


FrameRateLimiter::~FrameRateLimiter()
{
	// Empty
}


void FrameRateLimiter::setStartFrame()
{
	_startTime = SDL_GetTicks();
}


void FrameRateLimiter::LimitFPS(float* deltaTime)
{
	float currFrameTime = SDL_GetTicks() - _startTime;
	if (_MSPF > currFrameTime) {
		SDL_Delay(_MSPF - currFrameTime);
		currFrameTime = _MSPF;
	}
	*deltaTime = currFrameTime * _velocity;
}


void FrameRateLimiter::printFPS()
{
	static int frame = 0;
	frame++;
	if (frame % _printFrequency == 0)
	{
		std::cout << 1000.0f / (SDL_GetTicks() - _startTime) << std::endl;
		frame = 0;
	}
}