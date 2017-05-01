#include <SDL/SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <SDL/SDL_render.h>

#include "Window.h"
#include "SimpIO.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Camera.h"


int main(int argc, char** argv)
{
	Window window;
	window.init();

	std::string filepath;
	if (argc == 2)
	{
		filepath = std::string(argv[1]);
	}

	PolygonList* polygonData = new PolygonList();
	Lighting* lightEngine = new Lighting();

	// Read data from simp
	SimpIO file("./pageF.simp", lightEngine, polygonData);
	RenderArgs renderParams = file.Read();

	//Initialize renderer
	InputManager userInput;
	Camera camera;
	Renderer renderer(&window, lightEngine, polygonData, renderParams);
	renderer.renderData();

	bool running = true;
	const int FPS = 25;
	const float MSPF = 1000.0f / FPS;
	float deltaTime = 1.0f;

	float currFrameTime = 0.0f;

	// Main loop
	Uint32 start;
	while (running) 
	{
		start = SDL_GetTicks();

		running = userInput.pollForEvents();
		camera.updateCamera(userInput.getKeys(), renderer.getLightEngine(), deltaTime);
		
		renderer.setRenderModes(userInput.getKeys());
		renderer.setCameraMatrix(camera.getCameraMatrix());
		renderer.renderData();

		currFrameTime = SDL_GetTicks() - start;
		if (MSPF > currFrameTime) {
			SDL_Delay(MSPF - currFrameTime);
			currFrameTime = MSPF;
		}

		deltaTime = currFrameTime / MSPF;
		//std::cout << deltaTime << std::endl;
		//std::cout << 1000.0f / (SDL_GetTicks() - start) << std::endl;
	}

	//Exit
	SDL_Quit();
	return 0;
}