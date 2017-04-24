#include "SimpIO.h"
#include "Point.h"
#include "Polygon.h"
#include "ObjReader.h"
#include "utility.h"

#include <iostream>


/* SimpIO helper functions ----------------------------------------------------- */

void SimpIO::TransformToWorld(const DynamicArray<Vertex>& vertices, std::vector<std::vector<Vertex>>* transformed)
{
	std::vector<Vertex> vertexList;

	for (int i = 0; i < vertices.length(); i++)
	{
		Vertex newVertex = vertices[i];
		newVertex.pos.hgDivision();
		newVertex.pos_WS = _CTM * newVertex.pos;

		// Transform the normal with respect to WSC, if a nonzero normal provided
		if (newVertex.normal != vec4(0.0f, 0.0f, 0.0f, 0.0f))
		{
			newVertex.normal = newVertex.normal * _invCTM;
			newVertex.normal.w = 0.0f;  
			newVertex.normal.normalize();
		}

		vertexList.push_back(newVertex);
	}

	// Compute default face normal - behaviour will set line/point normals to 0.
	vec4 faceNormal = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if (vertexList.size() >= 3)
	{
		faceNormal = cross(vertexList[2].pos_WS, vertexList[1].pos_WS, vertexList[0].pos_WS);
	}

	// Provide a default face normal to a vertex if one is not specified
	for (unsigned int i = 0; i < vertexList.size(); i++)
	{
		if (vertexList[i].normal == vec4(0.0f, 0.0f, 0.0f, 0.0f))
		{
			vertexList[i].normal = faceNormal;
			vertexList[i].normal.w = 0.0f;
			vertexList[i].normal.normalize();
		}
	}

	transformed->push_back(vertexList);
}


Color SimpIO::computeAmbientLight(Color color)
{
	return Color((unsigned char)(color.r * _ambientColor.r),
				 (unsigned char)(color.g * _ambientColor.g),
				 (unsigned char)(color.b * _ambientColor.b));
}


// Report error and exit program
void ReportSyntaxError(std::vector<std::string> tokens, std::string errorMsg = "")
{
	std::cout << "Encountered syntax error in .simp script in the following line:" << std::endl;
	for (unsigned int i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << " ";
	}
	std::cout << std::endl;

	if (!errorMsg.empty())
	{
		std::cout << "\"" + errorMsg + "\"" << std::endl;
	}
	std::cout << "Exiting." << std::endl;
	exit(-1);
}


/* End helper functions ------------------------------------------------------------ */


SimpIO::SimpIO(std::string filepath,
	Lighting* lightEngine, PolygonList* polygons,
	mat4 CTM, mat4 CAMERA, FrustumParams f_params,
	float depthNear, float depthFar,
	Color_f ambientColor, Color depthColor, Color surfaceColor,
	bool wireFrame) :
	_wireFrame(wireFrame),
	_CTM(CTM),
	_depthNear(depthNear),
	_depthFar(depthFar),
	_ambientColor(ambientColor),
	_depthColor(depthColor),
	_surfaceColor(surfaceColor),
	_CAMERA(CAMERA),
	_polygons(polygons),
	_frustumParams(f_params)
{
	_currentFile.open(filepath);
	if (!_currentFile.is_open())
	{
		std::cout << "Failed to open file upon initialization of SimpIO object. Exiting." << std::endl;
		std::cout << "Filepath: " + filepath << std::endl;
		exit(-1);
	}

	_lightEngine = lightEngine;
}


SimpIO::~SimpIO()
{
	// Empty
}


void SimpIO::Interpret(const std::vector<std::string>& tokens)
{
	/* ------------------------------------------------------------------------- COMMENT */
	if (tokens[0][0] == '#')
	{
		return;
	}

	/* --------------------------------------------------------------------------- SCALE */
	else if (tokens[0] == "scale")
	{
		float x, y, z;
		if (tokens.size() == 4)
		{
			x = stof(tokens[1]);
			y = stof(tokens[2]);
			z = stof(tokens[3]);
			_CTM.scale(x, y, z);
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* -------------------------------------------------------------------------- ROTATE */
	else if (tokens[0] == "rotate")
	{
		Axis axis;
		if (tokens[1][0] >= 'X' && tokens[1][0] <= 'Z' && tokens.size() == 3)
		{
			axis = (Axis)tokens[1][0];
			float angle = stof(tokens[2]);
			_CTM.rotate(angle, axis);
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* ----------------------------------------------------------------------- TRANSLATE */
	else if (tokens[0] == "translate")
	{
		if (tokens.size() == 4)
		{
			float x, y, z;
			x = stof(tokens[1]);
			y = stof(tokens[2]);
			z = stof(tokens[3]);
			_CTM.translate(x, y, z);
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* ---------------------------------------------------------------------------- LINE */
	else if (tokens[0] == "line")
	{
		DynamicArray<Vertex> vertices;
		if (tokens.size() == 7)
		{
			vec4 pos1 = vec4(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			Vertex v1 = Vertex(pos1, pos1, vec4(0.0f, 0.0f, 0.0f, 0.0f), _surfaceColor);
			v1.color = computeAmbientLight(v1.color);
			vertices.append(v1);

			vec4 pos2 = vec4(stof(tokens[4]), stof(tokens[5]), stof(tokens[6]));
			Vertex v2 = Vertex(pos2, pos2, vec4(0.0f, 0.0f, 0.0f, 0.0f), _surfaceColor);
			v2.color = computeAmbientLight(v2.color);
			vertices.append(v2);
		}
		else if (tokens.size() == 13)
		{
			vec4 pos1 = vec4(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			Color c1 = Color((unsigned char)(255 * stof(tokens[4])),
							 (unsigned char)(255 * stof(tokens[5])),
							 (unsigned char)(255 * stof(tokens[6])));
			Vertex v1 = Vertex(pos1, pos1, vec4(0.0f, 0.0f, 0.0f, 0.0f), c1);
			v1.color = computeAmbientLight(v1.color);
			vertices.append(v1);

			vec4 pos2 = vec4(stof(tokens[7]), stof(tokens[8]), stof(tokens[9]));
			Color c2 = Color((unsigned char)(255 * stof(tokens[10])),
							 (unsigned char)(255 * stof(tokens[11])),
							 (unsigned char)(255 * stof(tokens[12])));
			Vertex v2 = Vertex(pos2, pos2, vec4(0.0f, 0.0f, 0.0f, 0.0f), c2);
			v2.color = computeAmbientLight(v2.color);
			vertices.append(v2);
		}
		else
		{
			ReportSyntaxError(tokens);
		}

		std::vector<std::vector<Vertex>> transformedVertices;
		TransformToWorld(vertices, &transformedVertices);

		for (unsigned int i = 0; i < transformedVertices.size(); i++)
		{
			if (transformedVertices[i].size() == 2)
			{
				_polygons->append(transformedVertices[i]);
			}
		}
	}

	/* ------------------------------------------------------------------------- POLYGON */
	else if (tokens[0] == "polygon")
	{
		DynamicArray<Vertex> vertices;
		if (tokens.size() == 10)
		{
			for (unsigned int i = 1; i < tokens.size(); i += 3)
			{
				vec4 position = vec4(stof(tokens[i]), stof(tokens[i + 1]), stof(tokens[i + 2]));
				Vertex vertex = Vertex(position, position, vec4(0.0f, 0.0f, 0.0f, 0.0f), _surfaceColor);
				vertices.append(vertex);
			}
		}
		else if (tokens.size() == 19)
		{
			for (unsigned int i = 1; i < tokens.size(); i += 6)
			{
				vec4 position = vec4(stof(tokens[i]), stof(tokens[i + 1]), stof(tokens[i + 2]));
				Color color = Color((unsigned char)(255 * stof(tokens[i + 3])),
									(unsigned char)(255 * stof(tokens[i + 4])),
									(unsigned char)(255 * stof(tokens[i + 5])));
				Vertex vertex = Vertex(position, position, vec4(0.0f, 0.0f, 0.0f, 0.0f), color);
				vertices.append(vertex);
			}
		}
		else
		{
			ReportSyntaxError(tokens, "Incorrect polygon parameterization");
		}

		std::vector<std::vector<Vertex>> transformedVertices;

		TransformToWorld(vertices, &transformedVertices);

		for (unsigned int j = 0; j < transformedVertices.size(); j++)
		{
			_polygons->append(transformedVertices[j]);
		}
	}

	/* ---------------------------------------------------------------------------- FILE */
	else if (tokens[0] == "file")
	{
		if (tokens.size() == 2)
		{
			SimpIO fileInclude("./" + tokens[1] + ".simp",
							   _lightEngine, _polygons, _CTM, _CAMERA, _frustumParams, _depthNear, _depthFar,
							   _ambientColor, _depthColor, _surfaceColor, _wireFrame);
			RenderArgs modifiedVals = fileInclude.Read();

			// Set any parameters modifed by the simp file
			_ambientColor = modifiedVals.ambientColor;
			_CAMERA = modifiedVals.CAMERA;
			_CTM = modifiedVals.CTM;
			_depthColor = modifiedVals.depthColor;
			_depthFar = modifiedVals.depthFar;
			_depthNear = modifiedVals.depthNear;
			_surfaceColor = modifiedVals.surfaceColor;
			_wireFrame = modifiedVals.wireFrame;
			_frustumParams = modifiedVals.f_params;
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* ---------------------------------------------------------------------------- WIRE */
	else if (tokens[0] == "wire")
	{
		_wireFrame = true;
	}

	/* -------------------------------------------------------------------------- FILLED */
	else if (tokens[0] == "filled")
	{
		_wireFrame = false;
	}

	/* ------------------------------------------------------------------------------- { */
	else if (tokens[0] == "{")
	{
		_matrixStack.push(_CTM);
	}

	/* ------------------------------------------------------------------------------- } */
	else if (tokens[0] == "}")
	{
		if (!_matrixStack.is_empty())
		{
			_CTM = _matrixStack.pop();
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* -------------------------------------------------------------------------- CAMERA */
	else if (tokens[0] == "camera")
	{
		if (tokens.size() == 7)
		{
			_CAMERA = _CTM;

			// Get eyepoint for lighting calculation
			vec4 eyePoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			eyePoint = _CTM * eyePoint;
			_lightEngine->setEyePoint(eyePoint);
			
			// Get viewing frustum settings
			_frustumParams = FrustumParams(stof(tokens[5]), stof(tokens[6]),
										   stof(tokens[1]), stof(tokens[2]),
										   stof(tokens[3]), stof(tokens[4]));
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* ----------------------------------------------------------------------------- OBJ */
	else if (tokens[0] == "obj")
	{
		std::vector<Vertex> vertices;
		std::vector<vec4> normals;
		std::vector<face> faces;
		_invCTM = _CTM.inverse();

		// Populate the lists with .obj data
		ObjReader object("./" + tokens[1] + ".obj", &vertices, &normals, &faces, _surfaceColor);
		object.Read();

		// Render the data
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			DynamicArray<Vertex> vertexList;
			for (unsigned int k = 0; k < faces[i].vertices.size(); k++)
			{
				Vertex currVertex = vertices[faces[i].vertices[k].vIndex];
				currVertex.normal = normals[faces[i].vertices[k].nIndex];
				vertexList.append(currVertex);
			}

			std::vector<std::vector<Vertex>> transformedVertices;
			TransformToWorld(vertexList, &transformedVertices);

			for (unsigned int j = 0; j < transformedVertices.size(); j++)
			{
				if (transformedVertices[j].size() >= 3)
				{
					_polygons->append(transformedVertices[j]);
				}
			}
		}
	}

	/* ------------------------------------------------------------------------- AMBIENT */
	else if (tokens[0] == "ambient")
	{
		_ambientColor = Color_f(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
		_lightEngine->setAmbientLight(_ambientColor);
	}

	/* --------------------------------------------------------------------------- DEPTH */
	else if (tokens[0] == "depth")
	{
		_depthNear = stof(tokens[1]);
		_depthFar = stof(tokens[2]);
		_depthColor = Color((unsigned char)(255 * stof(tokens[3])),
							(unsigned char)(255 * stof(tokens[4])),
							(unsigned char)(255 * stof(tokens[5])));
	}

	/* ------------------------------------------------------------------------- SURFACE */
	else if (tokens[0] == "surface")
	{
		if (tokens.size() == 6)
		{
			_surfaceColor = Color((unsigned char)(255 * stof(tokens[1])),
				(unsigned char)(255 * stof(tokens[2])),
				(unsigned char)(255 * stof(tokens[3])));
			_lightEngine->setSurfaceProperties(stof(tokens[4]), (int)stof(tokens[5]));
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* --------------------------------------------------------------------------- LIGHT */
	else if (tokens[0] == "light")
	{
		if (tokens.size() == 6)
		{
			vec4 origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			vec4 position = _CTM * origin;

			Color_f lightColor = Color_f(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			_lightEngine->addLightSource(position, stof(tokens[4]), stof(tokens[5]), lightColor);
		}
		else
		{
			ReportSyntaxError(tokens);
		}
	}

	/* --------------------------------------------------------------------------- PHONG */
	else if (tokens[0] == "phong")
	{
		_lightEngine->setLightModel(LIGHTMODEL::PHONG);
	}

	/* ------------------------------------------------------------------------- GOURAUD */
	else if (tokens[0] == "gouraud")
	{
		_lightEngine->setLightModel(LIGHTMODEL::GOURAUD);
	}

	/* ---------------------------------------------------------------------------- FLAT */
	else if (tokens[0] == "flat")
	{
		_lightEngine->setLightModel(LIGHTMODEL::FLAT);
	}

	/* ----------------------------------------------------------------- BAD INSTRUCTION */
	else
	{
		ReportSyntaxError(tokens, "Error: Invalid command");
	}
}


/*	Splits an std::string into individual symobls and stores in a vector array.
*	Strips away characters '(', ',' and ')'.
*/
void splitStrings(const std::string& currLine, std::vector<std::string>* tokens)
{
	std::string word;

	for (unsigned int i = 0; i < currLine.size(); i++)
	{
		if (currLine[i] != ' ' && currLine[i] != '\t' && currLine[i] != '\n' && currLine[i] != '\r')
		{
			word += currLine[i];
		}
		else if (!word.empty() && word[0] != '\n')
		{
			tokens->emplace_back(std::string(word));
			word = "";
		}
	}

	if (!word.empty())
	{
		tokens->emplace_back(std::string(word));
		word = "";
	}

	// Remove erroneous syntax
	for (unsigned int i = 0; i < tokens->size(); i++)
	{
		if ((*tokens)[i][0] == '(' || (*tokens)[i][0] == '"')
		{
			(*tokens)[i] = (*tokens)[i].substr(1, (*tokens)[i].length());
		}
		if ((*tokens)[i][(*tokens)[i].size() - 1] == ')' ||
			(*tokens)[i][(*tokens)[i].size() - 1] == ',' ||
			(*tokens)[i][(*tokens)[i].size() - 1] == '"')
		{
			(*tokens)[i] = (*tokens)[i].substr(0, (*tokens)[i].length() - 1);
		}
	}
}


RenderArgs SimpIO::Read()
{
	std::cout << "Reading vertex data.." << std::endl;

	if (!_currentFile.is_open())
	{
		std::cout << "File not open. Exiting." << std::endl;
		exit(-1);
	}

	std::vector<std::string> tokens;
	while (!_currentFile.eof())
	{
		std::string currLine;
		getline(_currentFile, currLine);

		// Strip away commas
		for (unsigned int i = 0; i < currLine.size(); i++)
		{
			if (currLine[i] == ',')
			{
				currLine[i] = ' ';
			}
		}

		// Read and interpret line
		splitStrings(currLine, &tokens);
		if (!tokens.empty())
		{
			Interpret(tokens);
		}

		tokens.clear();
	}
	_currentFile.close();

	std::cout << "Done reading file." << std::endl;

	return RenderArgs(_CTM, _CAMERA, _wireFrame, _depthNear, _depthFar, _ambientColor, _depthColor, _surfaceColor, _frustumParams);
}
