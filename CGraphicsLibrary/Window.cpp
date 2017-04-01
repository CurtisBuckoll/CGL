#include "Window.h"

#include <iostream>

Window::Window()
{
	_window = nullptr;
	_renderer = nullptr;
	_texture = nullptr;
}

Window::~Window()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
}

void Window::init()
{
	_width = 650;
	_height = 650;

	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_SHOWN);
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _width, _height);
	_pixels.resize(_width * _height * 4, 0);

	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);
}


void Window::clearBackground()
{
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);
}

void Window::RenderFrame()
{
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(_renderer);

	SDL_UpdateTexture(_texture, NULL, &_pixels[0], _width * 4);
	SDL_RenderCopy(_renderer, _texture, NULL, NULL);
	SDL_RenderPresent(_renderer);

	memset(&_pixels[0], 0, _width * _height * 4);
}


void Window::setPixel(int x, int y, Color color)
{
	int Y = -(y - (int)_height);
	int offset = (_width * 4 * Y) + x * 4;
	if (offset >= _pixels.size())
	{
		std::cout << "Out of range: " << offset << " " << _pixels.size() << std::endl;
		return;
	}
	_pixels[offset + 0] = color.r;      
	_pixels[offset + 1] = color.g;       
	_pixels[offset + 2] = color.b;       
	_pixels[offset + 3] = SDL_ALPHA_OPAQUE;
}
