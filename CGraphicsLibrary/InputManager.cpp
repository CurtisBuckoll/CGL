#include "InputManager.h"

#include <SDL\SDL.h>


InputManager::InputManager()
{
	for (unsigned int i = 0; i < SIZE; i++)
	{
		keys[i] = false;
	}
}


InputManager::~InputManager()
{
	//Empty
}


bool InputManager::pollForEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_KEYDOWN:
			keys[event.key.keysym.sym] = true;
			break;

		case SDL_KEYUP:
			keys[event.key.keysym.sym] = false;
			break;
		}
	}
	return true;
}


bool* InputManager::getKeys()
{
	return &keys[0];
}