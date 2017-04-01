#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "Line.h"
#include "Polygon.h"
#include "Matrix.h"
#include "zBuffer.h"
#include "Window.h"
#include "Vertex.h"

class Client
{
public:
    Client(Window *window, std::string filepath);
    void nextPage();

private:
    Window* _window;
    std::string _filePath;

    void RenderFromCommandLine();
    void draw_rect(int x1, int y1, int x2, int y2, Color color);

    /*	Assignment 3 subroutines  */
	void pageOne();
	void pageTwo();
	void pageThree();
	void pageFour();
	void pageFive();
	void pageSix();
	void pageSeven();
	void pageEight();
};

#endif // CLIENT_H
