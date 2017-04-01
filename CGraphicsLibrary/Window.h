#pragma once

#include <SDL\SDL.h>

class Window
{
private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;

public:
	Window();
	~Window();

	void init();
	void setPixel(int x, int y);
};

