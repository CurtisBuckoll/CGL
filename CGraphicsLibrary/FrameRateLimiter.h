#pragma once

#define NUM_FRAMES 10

class FrameRateLimiter
{
private:
	unsigned int _FPS;
	float _MSPF;
	float _velocity;
	unsigned int _startTime;

	unsigned int _printFrequency;
	float _frameTimes[NUM_FRAMES];
	int _frame;

public:
	FrameRateLimiter(unsigned int FPS, float velocity, unsigned int printFPS_frequency);
	~FrameRateLimiter();

	void setStartFrame();

	// Delay for appropriate frame time and modify deltaTime parameter accordingly
	void LimitFPS(float* deltaTime);

	void printFPS();
};

