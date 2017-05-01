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
#include "FrameRateLimiter.h"

std::string filepath = "./lightScene.simp";
const int FPS = 25;
const int PRINT_FPS_FREQ = 25;
const float MOVESPEED = 0.15f;

int main(int argc, char** argv)
{
	// Create window - size fixed at 650 x 650
	Window window;
	window.init();

	if (argc == 2)
	{
		filepath = std::string(argv[1]);
	}

	// Vertex data
	PolygonList* polygonData = new PolygonList();
	Lighting* lightEngine = new Lighting();

	// Read data from simp
	SimpIO file(filepath, lightEngine, polygonData);
	RenderArgs renderParams = file.Read();

	// Initialize renderer
	InputManager userInput;
	Camera camera;
	Renderer renderer(&window, lightEngine, polygonData, renderParams);

	// Initialize FPS limiter and main loop
	FrameRateLimiter fpsLimiter(FPS, MOVESPEED, PRINT_FPS_FREQ);
	bool running = true;
	float deltaTime = 1.0f;

	// Loop
	while (running) 
	{
		fpsLimiter.setStartFrame();

		running = userInput.pollForEvents();
		camera.updateCamera(userInput.getKeys(), renderer.getLightEngine(), deltaTime);
		
		renderer.setRenderModes(userInput.getKeys());
		renderer.setCameraMatrix(camera.getCameraMatrix());
		renderer.renderData();

		fpsLimiter.LimitFPS(&deltaTime);
		fpsLimiter.printFPS();

		//std::cout << deltaTime << std::endl;
	}

	SDL_Quit();
	return 0;
}