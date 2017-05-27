#pragma once

#define NUM_FRAMES 10

class FrameRateLimiter
{
private:
	unsigned int _FPS;
	double _MSPF;
	double _velocity;
	unsigned int _startTime;

	unsigned int _printFrequency;
	double _frameTimes[NUM_FRAMES];
	int _frame;

public:
	FrameRateLimiter(unsigned int FPS, double velocity, unsigned int printFPS_frequency);
	~FrameRateLimiter();

	void setStartFrame();

	// Delay for appropriate frame time and modify deltaTime parameter accordingly
	void LimitFPS(double* deltaTime);

	void printFPS();
};

