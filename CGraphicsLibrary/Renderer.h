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
	TRANSLATE_F,
	TRANSLATE_B
};

class Renderer
{
public:
	Renderer(RenderArgs parameters, Window* window, zBuffer* buffer, Lighting* _LightEngine, PolygonList* vertexData);
	~Renderer();

	void ToggleLighting();
	void UpdateCamera(CAM_INSTR instruction);
	void renderData();

private:
	Window* _window;
	bool _wireFrame;
	zBuffer* _zBuffer;
	Clip _frustum;

	float _depthNear;
	float _depthFar;
	Color_f _ambientColor;
	Color _depthColor;
	Color _surfaceColor;

	mat4 _CAMERA;
	mat4 _CAMERA_INVERSE;
	mat4 _PROJ;
	mat4 _SCREEN;

	PolygonList* _vertexData;

	Lighting* _lightEngine;

	void TransformToScreen(std::vector<Vertex>* vertices, std::vector<std::vector<Vertex>>* transformed);
};

