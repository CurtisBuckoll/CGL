#include <SDL/SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>


inline void setPixel(SDL_Window* screen, SDL_Renderer *renderer,  int x, int y)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
	SDL_RenderDrawPoint(renderer, x, y);
	SDL_RenderPresent(renderer); // Move this to it's own 'render frame' function


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

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_SHOWN
		);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	bool running = true;
	const int FPS = 30;
	Uint32 start;
	for (int i = 0; i < 100; i += 1) 
	{
		setPixel(window, renderer, i, 5);
	}

	while (running) 
	{
		start = SDL_GetTicks();
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		// Logic

		if (1000 / FPS > SDL_GetTicks() - start) {
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
		}
	}
	SDL_Quit();
	return 0;
}