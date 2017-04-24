#include "InputManager.h"



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
