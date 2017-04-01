#include "Window.h"



Window::Window()
{
	_window = nullptr;
	_renderer = nullptr;
}


Window::~Window()
{
}

void Window::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow("Window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		750, 750,
		SDL_WINDOW_SHOWN
		);

	_renderer = SDL_CreateRenderer(_window, -1, 0);

	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);
}


void Window::setPixel(int x, int y)
{
	SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 0);
	SDL_RenderDrawPoint(_renderer, x, y);
	SDL_RenderPresent(_renderer); // Move this to it's own 'render frame' function


	/*
	SDL_Texture *Tile = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 1, 1);
	unsigned char* bytes = nullptr;
	int pitch = 0;
	SDL_LockTexture(Tile, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
	unsigned char rgba[4] = { 255, 0, 0, 255 };
	memcpy(&bytes[0], rgba, sizeof(rgba));
	SDL_UnlockTexture(Tile);
	SDL_Rect destination = { x, y, 1, 1 };
	SDL_RenderCopy(renderer, Tile, NULL, &destination);
	SDL_RenderPresent(renderer);
	*/
}
