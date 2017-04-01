#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Matrix.h"
#include "Stack.h"
#include "zBuffer.h"
#include "Vertex.h"
#include "Lighting.h"
#include "Clip.h"
#include "Window.h"
#include "SimpIO.h"

enum class CAM_INSTR
{
	ROTATE_L,
	ROTATE_R,
	TRANSLATE_L,
	TRANSLATE_R,
};

class Renderer
{
public:
	Renderer(SimpIOArgs parameters, Window* window, zBuffer* buffer, Lighting* _LightEngine, PolygonList* vertexData);
	~Renderer();

	void UpdateCamera(CAM_INSTR instruction);
	void renderData();

private:
	Window* _window;
	bool _wireFrame;
	mat4 _SCREEN;
	Stack<mat4> _matrixStack;
	std::ifstream _currentFile;
	const float _windowSpaceMultiplier;
	zBuffer* _zBuffer;

	float _depthNear;
	float _depthFar;
	Color_f _ambientColor;
	Color _depthColor;
	Color _surfaceColor;
	mat4 _CAMERA;
	mat4 _CAMERA_INVERSE;
	mat4 _PROJ;
	Clip _frustum;
	PolygonList* _vertexData;

	Lighting* _lightEngine;

	//void transformVertices(vec4 vertex, vec4* WS_Coord, vec4* SS_Coord);
	void scaleToScreen(vec4* vertex);
	void TransformToScreen(std::vector<Vertex>* vertices, std::vector<std::vector<Vertex>>* transformed);
};

