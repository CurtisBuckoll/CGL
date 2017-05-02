#include "FrameRateLimiter.h"

#include <iostream>
#include <SDL\SDL.h>


FrameRateLimiter::FrameRateLimiter(unsigned int FPS, float velocity, unsigned int printFPSfreq)
{
	_FPS = FPS;
	_MSPF = 1000.0f / FPS;
	_velocity = velocity;
	_printFrequency = printFPSfreq;
	_frame = 0;
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
	float currFrameTime = static_cast<float>(SDL_GetTicks() - _startTime);

	if (_MSPF > currFrameTime) 
	{
		SDL_Delay(static_cast<Uint32>(_MSPF - currFrameTime));
		currFrameTime = _MSPF;
	}

	_frameTimes[_frame] = currFrameTime;
	_frame = (_frame + 1) % NUM_FRAMES;

	*deltaTime = currFrameTime * _velocity;
}


void FrameRateLimiter::printFPS()
{
	static int frameNumber;
	frameNumber++;

	if (frameNumber % _printFrequency == 0)
	{
		float avg = 0.0f;
		for (unsigned int i = 0; i < NUM_FRAMES; i++)
		{
			avg += _frameTimes[i];
		}
		avg = (1000.0f * NUM_FRAMES) / avg;

		std::cout << "FPS " << avg << std::endl;
	}
}