#include <SDL/SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <SDL/SDL_render.h>

#include "Window.h"
#include "SimpIO.h"
#include "Renderer.h"


int main(int argc, char** argv)
{
	Window window;
	window.init();

	std::string filepath;
	if (argc == 2)
	{
		filepath = std::string(argv[1]);
	}
	
	zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
	Lighting* lightEngine = new Lighting();
	PolygonList* polygonData = new PolygonList();

	SimpIO file("./lightScene.simp", zbuffer, lightEngine, polygonData);
	RenderArgs renderParams = file.Read();
	zbuffer = renderParams.zbuffer;

	Renderer renderer(renderParams, &window, zbuffer, lightEngine, polygonData);
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
				if (event.key.keysym.sym == SDLK_a)
				{
					renderer.UpdateCamera(CAM_INSTR::ROTATE_L);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_d)
				{
					renderer.UpdateCamera(CAM_INSTR::ROTATE_R);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					renderer.UpdateCamera(CAM_INSTR::TRANSLATE_L);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					renderer.UpdateCamera(CAM_INSTR::TRANSLATE_R);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_UP)
				{
					renderer.UpdateCamera(CAM_INSTR::TRANSLATE_F);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					renderer.UpdateCamera(CAM_INSTR::TRANSLATE_B);
					update = true;
				}
				if (event.key.keysym.sym == SDLK_l)
				{
					renderer.ToggleLighting();
					update = true;
				}
				break;
			}
		}
		
		if (update)
		{
			renderer.renderData();
			update = false;
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