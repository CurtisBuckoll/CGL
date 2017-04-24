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


struct PolygonList 
{
	std::vector<std::vector<Vertex>> vertices;

	void append(std::vector<Vertex> polygon)
	{
		vertices.push_back(polygon);
	}
};


struct RenderArgs {
	mat4 CTM;
	mat4 CAMERA;
	bool wireFrame;
	float depthNear;
	float depthFar;
	Color_f ambientColor;
	Color depthColor;
	Color surfaceColor;
	FrustumParams f_params;

	RenderArgs(mat4 CTM_, mat4 CAMERA_, bool wireframe_, float depthNear_, float depthFar_,
			   Color_f ambientColor_, Color depthColor_, Color surfaceColor_, FrustumParams f_params_) :
		CTM(CTM_),
		CAMERA(CAMERA_),
		wireFrame(wireframe_),
		depthNear(depthNear_),
		depthFar(depthFar_),
		ambientColor(ambientColor_),
		depthColor(depthColor_),
		surfaceColor(surfaceColor_),
		f_params(f_params_)
	{
		// Empty
	}
};


class SimpIO
{
public:
	SimpIO(std::string filepath,
		   Lighting* lightEngine, PolygonList* polygons,
		   mat4 CTM = mat4(), mat4 CAMERA = mat4(), FrustumParams f_params = FrustumParams(),
		   float depthNear = 3.402823466e38, float depthFar = 3.402823466e38,
		   Color_f ambientColor = Color_f(0.0f, 0.0f, 0.0f), Color depthColor = Color(0, 0, 0),
		   Color surfaceColor = Color(255, 255, 255), bool wireFrame = false);
	~SimpIO();
	RenderArgs Read();

private:
	mat4 _CTM;
	mat4 _invCTM;
	Stack<mat4> _matrixStack;
	std::ifstream _currentFile;

	bool _wireFrame;
	float _depthNear;
	float _depthFar;
	Color_f _ambientColor;
	Color _depthColor;
	Color _surfaceColor;
	mat4 _CAMERA;
	PolygonList* _polygons;
	Lighting* _lightEngine;
	FrustumParams _frustumParams;

	Color computeAmbientLight(Color color);
	void TransformToWorld(const DynamicArray<Vertex>& vertices, std::vector<std::vector<Vertex>>* transformed);
	void Interpret(const std::vector<std::string>& tokens);
};

