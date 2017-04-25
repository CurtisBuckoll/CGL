#pragma once

const int SIZE = 1024;

class InputManager
{
private:
	bool keys[SIZE];

public:
	InputManager();
	~InputManager();

	// Returns false if should exit
	bool pollForEvents();

	// Returns a pointer the keyboard map
	bool* getKeys();
};

