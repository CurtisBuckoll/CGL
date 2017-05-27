#pragma once

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

#include "Matrix.h"


void writeSphere(std::string fileName)
{
	std::ofstream file;
	file.open(fileName);

	file << "scale 30, 30, 100" << std::endl;
	file << "translate 60, 30, 200" << std::endl;

	mat4 rotateY;
	rotateY.rotate(18, Axis::Y);
	
	for (int i = 19; i >= -19; i--)
	{
		double y = (double)i / 20;
		vec4 v1 = vec4(sqrt(1 - pow(y + 0.05f, 2)), (y + 0.05f), 0.0);
		vec4 v2 = vec4(sqrt(1 - pow(y, 2)), y, 0.0);
		vec4 v3 = vec4(sqrt(1 - pow(y - 0.05f, 2)), (y - 0.05f), 0.0);

		for (int k = 0; k < 20; k++)
		{
			vec4 v4 = rotateY * v1;
			vec4 v5 = rotateY * v2;

			std::string vertex2 = std::string(" (" + std::to_string(v2.x) + ", " + std::to_string(v2.y) + ", " + std::to_string(v2.z) + ")");
			std::string vertex3 = std::string(" (" + std::to_string(v3.x) + ", " + std::to_string(v3.y) + ", " + std::to_string(v3.z) + ")");
			std::string vertex4 = std::string(" (" + std::to_string(v4.x) + ", " + std::to_string(v4.y) + ", " + std::to_string(v4.z) + ")");
			std::string vertex5 = std::string(" (" + std::to_string(v5.x) + ", " + std::to_string(v5.y) + ", " + std::to_string(v5.z) + ")");

			file << "polygon" + vertex4 + vertex2 + vertex5 << std::endl;
			file << "polygon" + vertex2 + vertex3 + vertex5 << std::endl;

			v1 = rotateY * v1;
			v2 = rotateY * v2;
			v3 = rotateY * v3;
		}

		file << std::endl;
	}

	file.close();
}