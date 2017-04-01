#pragma once

#include <drawable.h>

class DrawableCustom : Drawable
{
public:
	DrawableCustom();
	DrawableCustom(int xLeft, int yBottom, int width, int height, Drawable* parent);
	void setPixel(int x, int y, unsigned int color);
	unsigned int getPixel(int x, int y);
	Drawable* getDrawable();

private:
	Drawable* _drawable;
	int _xLeft;
	int _yBottom;
	int _width;
	int _height;
	
	int _leftBound;
	int _rightBound;
	int _topBound;
	int _botBound;

	void updateScreen();
};
