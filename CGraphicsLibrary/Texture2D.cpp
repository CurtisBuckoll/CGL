#include "Texture2D.h"

#include <iostream>
#include <fstream>

#include "picoPNG.h"


void loadFile(std::vector<unsigned char>& data, const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	// Find filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good())
	{
		size = file.tellg();
	}
	if (file.seekg(0, std::ios::beg).good())
	{
		size -= file.tellg();
	}

	// Read file into vector
	if (size > 0)
	{
		data.resize((size_t)size);
		file.read((char*)(&data[0]), size);
	}
	else
	{
		data.clear();
	}
}



Texture2D::Texture2D()
{
	_filepath = "";
}


Texture2D::Texture2D(const std::string& filepath)
{
	_filepath = filepath;
	std::vector<unsigned char> pngData;
	loadFile(pngData, filepath);

	if (pngData.size() <= 0)
	{
		std::cout << "Error: " + filepath + " failed to load image. Quitting." << std::endl;
		exit(-1);
	}

	int errorCode = decodePNG(_imgData, _width, _height, &pngData[0], (size_t)pngData.size(), true);
	if (errorCode != 0)
	{
		std::cout << "Error: " + filepath + " failed to decode. Quitting." << std::endl;
		exit(-1);
	}
}


Texture2D::~Texture2D()
{
	//Empty
}


RGBA Texture2D::getTexel(unsigned int x, unsigned int y)
{
	unsigned long base = (y * 4 * _width) + (x * 4);

	if (base >= _imgData.size())
	{
		//std::cout << "Texel out of range: " << base << " " << _imgData.size() << std::endl;
		return RGBA(255, 255, 255, 255);
	}

	return  RGBA(_imgData[base + 2],	// Data stored BGRA ordering
				 _imgData[base + 1],
				 _imgData[base], 
				 _imgData[base + 3]);
}
