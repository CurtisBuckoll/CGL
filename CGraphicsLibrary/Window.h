#pragma once

#include <SDL\SDL.h>

#include "Vertex.h"

class Window
{
private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;

public:
	Window();
	~Window();

	void init();
	void clearBackground();
	void setPixel(int x, int y, Color color);
	void RenderFrame();
};

