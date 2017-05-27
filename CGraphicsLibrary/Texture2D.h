#pragma once

#include <vector>
#include <string>


struct RGBA {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	RGBA(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
};


class Texture2D
{
private:
	std::string _filepath;
	std::vector<unsigned char> _imgData;
	unsigned long _width;
	unsigned long _height;

public:
	Texture2D();
	Texture2D(const std::string& filename);
	~Texture2D();

	RGBA getTexel(unsigned int x, unsigned int y);
};

