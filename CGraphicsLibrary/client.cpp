#include "client.h"

#include <iostream>
#include <random>
#include <SDL\SDL.h>

#include "SimpReader.h"
#include "DynamicArray.h"
#include "Stack.h"
#include "Matrix.h"
#include "lighting.h"


/* Client helper functions --------------------------------------------------------- */


void stackTest()
{
	Stack<int> stack;
	for (int i = 0; i < 12; i++)
	{
		stack.push(i);
	}

	std::cout << "Size: " << stack.size() << std::endl;
	while (!stack.is_empty())
	{
		std::cout << "Is empty : " << stack.is_empty() << std::endl;
		std::cout << "Top: " << stack.top() << std::endl;
		std::cout << "Pop: " << stack.pop() << std::endl << std::endl;
	}
	std::cout << "is empty: " << stack.is_empty() << std::endl;
	std::cout << "Size: " << stack.size() << std::endl;

	for (int i = 0; i < 12; i++)
	{
		stack.push(i);
	}

	stack.clear();
	std::cout << "Should be empty: " << stack.is_empty() << std::endl;
	std::cout << "Should have zero size: " << stack.size() << std::endl;
}

/* End helper functions ------------------------------------------------------------ */


Client::Client(Window *window, std::string filepath)
{
	_window = window;
    _filePath = filepath;
}


void Client::pageOne()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./lightScene.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}


void Client::pageTwo()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./lightScene2.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}


void Client::pageThree()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./lightScene3.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}


void Client::pageFour()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./cam4.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}


void Client::pageFive()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./cam5.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}

void Client::pageSix()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./cam6.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}

void Client::pageSeven()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./cam7.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}

void Client::pageEight()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./cam8.simp", _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}

void Client::RenderFromCommandLine()
{
	_window->clearBackground();

    zBuffer* zbuffer = new zBuffer(650, 650, 1.0f, 200.0f);
    Lighting* lightEngine = new Lighting();

    SimpReader file("./" + _filePath, _window, zbuffer, lightEngine);
    file.Read();

	_window->RenderFrame();
}


void Client::nextPage() 
{
    static int pageNumber = 0;
    std::cout << "PageNumber " << (pageNumber % 8) + 1 << std::endl;


    if (_filePath != "")
    {
        RenderFromCommandLine();
        return;
    }

    switch(pageNumber % 8) 
	{
    case 0:
        //invert_test();
		pageOne();
        break;

    case 1:
		pageTwo();
        break;

    case 2:
		pageThree();
        break;

    case 3:
		pageFour();
		break;

	case 4:
		pageFive();
		break;

	case 5:
		pageSix();
		break;

	case 6:
		pageSeven();
		break;

	case 7:
		pageEight();
		break;

    default:
        draw_rect(0, 0, 750, 750, Color (255, 255, 255));
        draw_rect(400, 400, 700, 700, Color(0, 0, 0));
        //_drawable->updateScreen();
    }

	pageNumber++;
}


void Client::draw_rect(int x1, int y1, int x2, int y2, Color color) 
{
    for(int x = x1; x<x2; x++) 
	{
        for(int y=y1; y<y2; y++) 
		{
            _window->setPixel(x, y, color);
        }
    }
}
