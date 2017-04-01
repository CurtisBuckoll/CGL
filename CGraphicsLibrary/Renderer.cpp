#include "Renderer.h"
#include "Point.h"
#include "Polygon.h"
#include "ObjReader.h"
#include "utility.h"

#include <iostream>


void Renderer::TransformToScreen(std::vector<Vertex>* vertices, std::vector<std::vector<Vertex>>* transformed)
{
	std::vector<Vertex> vertexList;


	// Transform to camera space
	for (int i = 0; i < vertices->size(); i++)
	{
		Vertex newVertex = (*vertices)[i];
		newVertex.pos = _CAMERA_INVERSE * newVertex.pos_WS;
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
		for (int k = 1; k < vertexList.size() - 1; k++)
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
		for (int k = 0; k < vertexList.size(); k++)           //This doesn't have to be loop. change later
		{
			line.push_back(vertexList[k]);
		}
		splitVertexList.push_back(line);
	}

	// Finish transforming and render the data
	for (int j = 0; j < splitVertexList.size(); j++)
	{
		//DynamicArray<Vertex> points;
		for (int k = 0; k < splitVertexList[j].size(); k++)
		{
			splitVertexList[j][k].pos = _PROJ * splitVertexList[j][k].pos;
			if (splitVertexList[j][k].pos.z != 0)
			{
				splitVertexList[j][k].pos.x = splitVertexList[j][k].pos.x / splitVertexList[j][k].pos.z;
				splitVertexList[j][k].pos.y = splitVertexList[j][k].pos.y / splitVertexList[j][k].pos.z;
			}
			splitVertexList[j][k].pos.w = 1.0f;
			scaleToScreen(&splitVertexList[j][k].pos);
			splitVertexList[j][k].pos = _SCREEN * splitVertexList[j][k].pos;
			//splitVertexList[j][k].color = computeAmbientLight(splitVertexList[j][k].color);
		}
		transformed->push_back(splitVertexList[j]);
	}
}


void Renderer::scaleToScreen(vec4* vertex)
{
	if (vertex->x < 0)
	{
		vertex->x *= (float)_zBuffer->width / (2 * fabs(_zBuffer->xLo));
	}
	else
	{
		vertex->x *= (float)_zBuffer->width / (2 * fabs(_zBuffer->xHi));
	}

	if (vertex->y < 0)
	{
		vertex->y *= (float)_zBuffer->height / (2 * fabs(_zBuffer->yLo));
	}
	else
	{
		vertex->y *= (float)_zBuffer->height / (2 * fabs(_zBuffer->yHi));
	}
}


void Renderer::UpdateCamera(CAM_INSTR instruction)
{
	const float moveAmount = 5.0f;
	mat4 updateMatrix = mat4();

	switch (instruction)
	{
	case CAM_INSTR::ROTATE_L :
		updateMatrix.rotate(25.0f, Axis::Y);
		_CAMERA = _CAMERA * updateMatrix;
		break;

	case CAM_INSTR::ROTATE_R:
		updateMatrix.rotate(-25.0f, Axis::Y);
		_CAMERA = _CAMERA * updateMatrix;
		break;

	case CAM_INSTR::TRANSLATE_L:
		updateMatrix.translate(-moveAmount, 0.0, 0.0f);
		_CAMERA = updateMatrix * _CAMERA;
		break;
	case CAM_INSTR::TRANSLATE_R:
		updateMatrix.translate(moveAmount, 0.0, 0.0f);
		_CAMERA = updateMatrix * _CAMERA;
		break;
	}

	vec4 eyePoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	eyePoint = _CAMERA * eyePoint;
	_lightEngine->setEyePoint(eyePoint);
}


/* End helper functions ------------------------------------------------------------ */


Renderer::Renderer(SimpIOArgs parameters, Window* window, zBuffer* buffer, Lighting* _LightEngine, PolygonList* vertexData) :
	_wireFrame(parameters.wireFrame),
	_windowSpaceMultiplier(3.25f),
	_depthNear(parameters.depthNear),
	_depthFar(parameters.depthFar),
	_ambientColor(parameters.ambientColor),
	_depthColor(parameters.depthColor),
	_surfaceColor(parameters.surfaceColor),
	_CAMERA(parameters.CAMERA),
	_PROJ(MAT_TYPE::PROJECTION)
{
	_window = window;
	_zBuffer = buffer;
	_lightEngine = _LightEngine;
	_CAMERA_INVERSE = mat4();
	_vertexData = vertexData;

	_frustum = Clip();
	_frustum.Init(buffer->xLo, buffer->yLo, buffer->xHi, buffer->yHi, buffer->hither, buffer->yon);
	_SCREEN.translate(((float)_zBuffer->width / 2), ((float)_zBuffer->height / 2), 0.0f);
}


Renderer::~Renderer()
{
	// Empty
}


void Renderer::renderData()
{
	_window->clearBackground();
	_CAMERA_INVERSE = _CAMERA.inverse();

	
	for (int i = 0; i < _vertexData->vertices.size(); i++)
	{

		std::vector<std::vector<Vertex>> transformedVertices;


		TransformToScreen(&(_vertexData->vertices[i]), &transformedVertices);
		
		for (int j = 0; j < transformedVertices.size(); j++)
		{
			if (transformedVertices[j].size() == 2)
			{
				Line::DDA(transformedVertices[j][0], transformedVertices[j][1], _zBuffer, _window);
			}
			else if (transformedVertices[j].size() >= 3)
			{
				_lightEngine->init(&transformedVertices[j]);
				Polygon::drawPolygonLERP(transformedVertices[j], 1.0f, _wireFrame, _zBuffer, _window, _lightEngine);
			}
		}
	}
	_zBuffer->reset();
	_window->RenderFrame();
}