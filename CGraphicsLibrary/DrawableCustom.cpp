#include "DrawableCustom.h"

#include <iostream>

// Invert Y for Qt to draw starting at top-left of window at (0,0)
int inline invertY(int coord)
{
	return -(coord - 750);
}

DrawableCustom::DrawableCustom()
{
	_drawable = nullptr;
	_xLeft = 0;
	_yBottom = 0;
}

DrawableCustom::DrawableCustom(int xLeft, int yBottom, int width, int height, Drawable* parent) : 
	Drawable(),
	_width(width),
	_height(height)
{
	_drawable = parent;
	_xLeft = xLeft;
	_yBottom = yBottom;

	_leftBound = xLeft;
	_rightBound = xLeft + width;
	_topBound = yBottom + height;
	_botBound = yBottom;
}

void inline DrawableCustom::setPixel(int x, int y, unsigned int color) 
{
	int X = x + _xLeft;
	int Y = y + _yBottom + 1;
	_drawable->setPixel(X, invertY(Y), color);

}

unsigned int DrawableCustom::getPixel(int x, int y) 
{
	return _drawable->getPixel(x + _xLeft, invertY(y + _yBottom + 1));
}

Drawable* DrawableCustom::getDrawable()
{
	return this;
}

void DrawableCustom::updateScreen()
{
	// Do nothing.
}
