#pragma once

#include "Matrix.h"
#include "Vertex.h"
#include <vector>


enum class LIGHTMODEL {
    FLAT = 0,
    PHONG,
    GOURAUD
};


struct LightSource {
    /** These are set when we initialise a light source **/
    vec4 position;
    double A;
    double B;			// A, B are attenuation constants from "light" command
    Color_f I;			// Intesity of light i

    // For FLAT/GOURAUD shading
    double diffuse;
    double specular;
    double f_att;

    /** Methods	**/
    LightSource(const vec4& pos, double A, double B, const Color_f& intensity);
    LightSource();
    ~LightSource();
};


class Lighting {
private:
    LIGHTMODEL _model;
    std::vector<LightSource> _lightSources;

    double _k_s;         // Specular reflection coefficient
    int _p;             // Specular reflection exponent
    vec4 _eyepoint;
    Color_f _ambient;

    Color ComputeVertexShadingColor(Color objectColor);

public:
	bool doLighting;

    Lighting();
    ~Lighting();

    void setSurfaceProperties(double K_s, int P);
    void setAmbientLight(const Color_f& color);
    void setLightModel(LIGHTMODEL model);
    void setEyePoint(const vec4& eyePoint);
    void addLightSource(const vec4& pos, double A, double B, const Color_f& intensity);

    void init(std::vector<Vertex>* vertices);
    Color PerformLightingCalculation(Color objectColor, vec4& N, vec4& vertexPos);
    
};






