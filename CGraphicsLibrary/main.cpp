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

// Global parameters
std::string FILEPATH = "./lightScene.simp";
const int WIN_WIDTH = 650;
const int WIN_HEIGHT = 650;
const int FPS = 25;
const int PRINT_FPS_FREQ = 25;
const float MOVESPEED = 0.10f;

// Entry
int main(int argc, char** argv)
{
	// Create window
	Window window(WIN_WIDTH, WIN_HEIGHT);
	window.init();

	if (argc == 2)
	{
		FILEPATH = std::string(argv[1]);
	}

	// Vertex data
	PolygonList* polygonData = new PolygonList();
	Lighting* lightEngine = new Lighting();

	// Read data from simp
	SimpIO file(FILEPATH, lightEngine, polygonData);
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
	}

	SDL_Quit();
	return 0;
}