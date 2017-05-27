#include "Renderer.h"
#include "Point.h"
#include "Polygon.h"
#include "ObjReader.h"
#include "utility.h"

#include <iostream>
#include <algorithm>


Renderer::Renderer(Window* window, Lighting* _LightEngine, PolygonList* vertexData, RenderArgs parameters) :
	_wireFrame(parameters.wireFrame),
	_depthNear(parameters.depthNear),
	_depthFar(parameters.depthFar),
	_ambientColor(parameters.ambientColor),
	_depthColor(parameters.depthColor),
	_surfaceColor(parameters.surfaceColor),
	_CAMERA(parameters.CAMERA),
	_PROJ(MAT_TYPE::PROJECTION)
{
	_window = window;
	_lightEngine = _LightEngine;
	_CAMERA_INVERSE = mat4();
	_vertexData = vertexData;

	_zBuffer = new zBuffer(window->getWidth(), window->getHeight(), parameters.depthNear, parameters.depthFar, parameters.depthColor);
	_frustum = Clip();
	_frustum.Init(parameters.f_params);

	// Set up screen matrix
	_SCREEN = mat4();
	double uniformScaleSize = (double)std::max(_frustum.parameters.xHi - _frustum.parameters.xLo, _frustum.parameters.yHi - _frustum.parameters.yLo);
	_SCREEN.translate((650.0 - (_frustum.parameters.xHi - _frustum.parameters.xLo) * 650.0 / uniformScaleSize) / 2.0f,
					  (650.0 - (_frustum.parameters.yHi - _frustum.parameters.yLo) * 650.0 / uniformScaleSize) / 2.0f - 1.0, 0.0);
	_SCREEN.scale(650.0 / uniformScaleSize, (650.0) / uniformScaleSize, 1.0);
	_SCREEN.translate(-_frustum.parameters.xLo, -_frustum.parameters.yLo, 0.0);

	_forward = vec4(0.0, 0.0, 1.0, 0.0);
	_right = vec4(1.0, 0.0, 0.0, 0.0);
}


Renderer::~Renderer()
{
	// Empty
}


void Renderer::TransformToScreen(std::vector<Vertex>* vertices, std::vector<std::vector<Vertex>>* transformed)
{
	std::vector<Vertex> vertexList;

	// Transform to camera space
	for (unsigned int i = 0; i < vertices->size(); i++)
	{
		Vertex newVertex = (*vertices)[i];
		newVertex.pos = _CAMERA * newVertex.pos_WS;
		vertexList.push_back(newVertex);
	}

	// Perform polygon culling/clipping
	if (_frustum.cullToFrustum(vertexList))
	{
		return;
	}
	_frustum.clipToFrustum(&vertexList);

	// Triangulate cipped vertices (if necessary)
	std::vector<std::vector<Vertex>> splitVertexList;
	if (vertexList.size() >= 3)
	{
		for (unsigned int k = 1; k < vertexList.size() - 1; k++)
		{
			std::vector<Vertex> triangle;
			triangle.push_back(vertexList[0]);
			triangle.push_back(vertexList[k]);
			triangle.push_back(vertexList[k + 1]);
			splitVertexList.push_back(triangle);
		}
	}
	else
	{
		std::vector<Vertex> line;
		for (unsigned int k = 0; k < vertexList.size(); k++)
		{
			line.push_back(vertexList[k]);
		}
		splitVertexList.push_back(line);
	}

	// Project data to viewing plane
	for (unsigned int j = 0; j < splitVertexList.size(); j++)
	{
		for (unsigned int k = 0; k < splitVertexList[j].size(); k++)
		{
			splitVertexList[j][k].pos = _PROJ * splitVertexList[j][k].pos;
			splitVertexList[j][k].pos.x = splitVertexList[j][k].pos.x / splitVertexList[j][k].pos.z;
			splitVertexList[j][k].pos.y = splitVertexList[j][k].pos.y / splitVertexList[j][k].pos.z;

			splitVertexList[j][k].pos.w = 1.0;
			splitVertexList[j][k].pos = _SCREEN * splitVertexList[j][k].pos;
		}

		// Perform backface culling
		if (!_frustum.backFaceCull(splitVertexList[j]))
		{
			transformed->push_back(splitVertexList[j]);
		}
	}
}


void Renderer::ToggleLighting()
{
	_lightEngine->doLighting = !_lightEngine->doLighting;
}


void Renderer::ToggleWireframe()
{
	_wireFrame = !_wireFrame;
}


void Renderer::setRenderModes(bool* keys)
{
	static bool shouldPress = true;
	if (keys[SDLK_l] && shouldPress)
	{
		ToggleLighting();
		shouldPress = false;
	}
	else if (!keys[SDLK_l])
	{
		shouldPress = true;
	}

	static bool shouldPressWire = true;
	if (keys[SDLK_f] && shouldPressWire)
	{
		ToggleWireframe();
		shouldPressWire = false;
	}
	else if (!keys[SDLK_f])
	{
		shouldPressWire = true;
	}
}


Lighting* Renderer::getLightEngine()
{
	return _lightEngine;
}


void Renderer::setCameraMatrix(mat4 camera)
{
	_CAMERA = camera;
}


void Renderer::renderData()
{
	_window->clearBackground();
	
	for (unsigned int i = 0; i < _vertexData->vertices.size(); i++)
	{
		std::vector<std::vector<Vertex>> transformedVertices;

		TransformToScreen(&(_vertexData->vertices[i]), &transformedVertices);
		
		for (unsigned int j = 0; j < transformedVertices.size(); j++)
		{
			if (transformedVertices[j].size() == 2)
			{
				Line::DDA(transformedVertices[j][0], transformedVertices[j][1], _zBuffer, _window);
			}
			else if (transformedVertices[j].size() >= 3)
			{
				_lightEngine->init(&transformedVertices[j]);
				Polygon::drawPolygonLERP(transformedVertices[j], 1.0, _wireFrame, _zBuffer, _window, _lightEngine);
			}
		}
	}

	_zBuffer->reset();
	_window->RenderFrame();
}