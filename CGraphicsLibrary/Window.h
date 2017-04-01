#pragma once

#include <SDL\SDL.h>

#include "Vertex.h"
#include <vector>

class Window
{
private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	SDL_Texture* _texture;
	std::vector<unsigned char> _pixels;


	unsigned int _width;
	unsigned int _height;

public:
	Window();
	~Window();

	void init();
	void clearBackground();
	void setPixel(int x, int y, Color color);
	void RenderFrame();
};

