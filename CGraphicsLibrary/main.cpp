#include <SDL/SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <SDL/SDL_render.h>

#include "Window.h"
#include "SimpIO.h"
#include "Renderer.h"
#include "InputManager.h"


int main(int argc, char** argv)
{
	Window window;
	window.init();

	std::string filepath;
	if (argc == 2)
	{
		filepath = std::string(argv[1]);
	}

	Lighting* lightEngine = new Lighting();
	PolygonList* polygonData = new PolygonList();

	SimpIO file("./pageF.simp", lightEngine, polygonData);
	RenderArgs renderParams = file.Read();

	Renderer renderer(&window, lightEngine, polygonData, renderParams);
	renderer.renderData();

	bool running = true;
	const int FPS = 30;


	// Keep window open
	Uint32 start;
	bool update = false;
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

			case SDL_KEYDOWN:
				renderer.userInput.keys[event.key.keysym.sym] = true;
				break;

			case SDL_KEYUP:
				renderer.userInput.keys[event.key.keysym.sym] = false;
				break;
			}
		}
		renderer.UpdateCamera();


		renderer.renderData();

		if (1000 / FPS > SDL_GetTicks() - start) {
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
		}

		std::cout << 1000.0f / (SDL_GetTicks() - start) << std::endl;
	}

	//Exit
	SDL_Quit();
	return 0;
}