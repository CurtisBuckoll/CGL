#include <SDL/SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>

#include "Window.h"
#include "client.h"
#include <SDL/SDL_render.h>


int main(int argc, char** argv)
{
	Window window;
	window.init();


	std::string filepath;
	if (argc == 2)
	{
		filepath = std::string(argv[1]);
	}

	Client client(&window, filepath);
	client.nextPage();


	bool running = true;
	const int FPS = 30;
	for (int i = 0; i < 100; i += 1) 
	{
		window.setPixel(i, 5, Color(255, 0, 0));
	}


	// Keep window open
	Uint32 start;
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

	//Exit
	SDL_Quit();
	return 0;
}