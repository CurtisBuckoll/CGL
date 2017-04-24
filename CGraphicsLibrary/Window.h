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


	int _width;
	int _height;

public:
	Window();
	~Window();

	int getWidth();
	int getHeight();

	void init();
	void clearBackground();
	void setPixel(int x, int y, Color color);
	void RenderFrame();
};

