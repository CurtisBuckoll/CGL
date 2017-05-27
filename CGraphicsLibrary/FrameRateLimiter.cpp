#include "FrameRateLimiter.h"

#include <iostream>
#include <SDL\SDL.h>


FrameRateLimiter::FrameRateLimiter(unsigned int FPS, double velocity, unsigned int printFPSfreq)
{
	_FPS = FPS;
	_MSPF = 1000.0 / FPS;
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


void FrameRateLimiter::LimitFPS(double* deltaTime)
{
	double currFrameTime = static_cast<double>(SDL_GetTicks() - _startTime);

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
		double avg = 0.0;
		for (unsigned int i = 0; i < NUM_FRAMES; i++)
		{
			avg += _frameTimes[i];
		}
		avg = (1000.0 * NUM_FRAMES) / avg;

		std::cout << "FPS " << avg << std::endl;
	}
}