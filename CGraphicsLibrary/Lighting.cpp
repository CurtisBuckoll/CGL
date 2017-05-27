#include "Lighting.h"

#include "utility.h"

#include <iostream>


/* LightSource Methods ------------------------------------------------------------- */


LightSource::LightSource()
{
    position = vec4(0.0, 0.0, 0.0, 0.0);
    A = 0.0;
    B = 0.0;
    I = Color_f();
}


LightSource::LightSource(const vec4& pos, double A, double B, const Color_f& intensity)
{
    position = pos;
    position.hgDivision();
    position.w = 0.0;
    this->A = A;
    this->B = B;
    I = intensity;

    diffuse = 1.0;
    specular = 1.0;
    f_att = 1.0;
}


LightSource::~LightSource()
{
    //Empty
}


Color Lighting::ComputeVertexShadingColor(Color objectColor)
{
    Color_f object_fl = objectColor.convertTodouble();
    Color_f result = object_fl;
    result = result * _ambient;

    for (unsigned int i = 0; i < _lightSources.size(); i++)
    {
        // Light does not hit face if 0
        if (_lightSources[i].diffuse <= 0) { continue; }

        // R
        double I_r = _lightSources[i].I.r;
        double k_r = object_fl.r;
        double R = I_r * _lightSources[i].f_att * (k_r * _lightSources[i].diffuse + _lightSources[i].specular);
        // G
        double I_g = _lightSources[i].I.g;
        double k_g = object_fl.g;
        double G = I_g * _lightSources[i].f_att * (k_g * _lightSources[i].diffuse + _lightSources[i].specular);
        // B
        double I_b = _lightSources[i].I.b;
        double k_b = object_fl.b;
        double B = I_b * _lightSources[i].f_att * (k_b * _lightSources[i].diffuse + _lightSources[i].specular);

        result.r += R;
        result.g += G;
        result.b += B;
    }

    if (result.r > 1.0) {result.r = 1.0;}
    if (result.g > 1.0) {result.g = 1.0;}
    if (result.b > 1.0) {result.b = 1.0;}

    return Color((unsigned char)(255 * result.r),
                 (unsigned char)(255 * result.g),
                 (unsigned char)(255 * result.b));
}


/* Lighting Methods ---------------------------------------------------------------- */


Lighting::Lighting()
{
    _model = LIGHTMODEL::FLAT;
    _k_s = 0.3f;
    _p = 8;
    _ambient = Color_f(0.0, 0.0, 0.0);
	doLighting = true;
}


Lighting::~Lighting()
{
    _lightSources.clear();
}


void Lighting::setSurfaceProperties(double K_s, int P)
{
    _k_s = K_s;
    _p = P;
}


void Lighting::setAmbientLight(const Color_f& color)
{
    _ambient = color;
}


void Lighting::setLightModel(LIGHTMODEL model)
{
    _model = model;
}


void Lighting::setEyePoint(const vec4& eyePoint)
{
    _eyepoint = eyePoint;
    _eyepoint.hgDivision();
    _eyepoint.w = 0.0;
}


void Lighting::addLightSource(const vec4& pos, double A, double B, const Color_f& intensity)
{
    _lightSources.push_back(LightSource(pos, A, B, intensity));
}


void Lighting::init(std::vector<Vertex>* vertices)
{
	if (!doLighting) { return; }

    if (vertices->size() != 3)
    {
        std::cout << "Wrong vertex array size for lighting init.. Does not equal 3" << std::endl;
        std::cout << vertices->size() << std::endl;
    }

    vec4 N;
    vec4 V;
    vec4 centerPos;

    switch (_model)
    {
    /* ------------------------------------------------------------------------ FLAT */
    case LIGHTMODEL::FLAT :

        // Compute center of polygon and average normal N to the face
        centerPos = vec4(0.0, 0.0, 0.0, 0.0);
        N = vec4(0.0, 0.0, 0.0, 0.0);
        for (unsigned int i = 0; i < vertices->size(); i++)
        {
            centerPos = centerPos + (*vertices)[i].pos_WS;
            N = N + (*vertices)[i].normal;
        }
        centerPos = centerPos * (1.0 / 3);
        centerPos.w = 0;
        N = N * (1.0 / 3);
        N.normalize();

        V = _eyepoint - centerPos;
        V.normalize();

        for (unsigned int i  = 0; i < _lightSources.size(); i++)
        {
            vec4 L = _lightSources[i].position - centerPos;
            double dist = L.length();
            _lightSources[i].f_att =  1.0 / (_lightSources[i].A + _lightSources[i].B * dist);
            L.normalize();
            vec4 R;
            R = (N * 2.0f * (N.dot(L))) - L;
            R.normalize();

            _lightSources[i].diffuse = N.dot(L);

			// Clamp V.R if < 0
			double VdotR = V.dot(R);
			if (VdotR < 0.0) { VdotR = 0.0; }

            _lightSources[i].specular = _k_s * pow(V.dot(R), _p);
        }

        (*vertices)[0].color = ComputeVertexShadingColor((*vertices)[0].color);
        (*vertices)[1].color = ComputeVertexShadingColor((*vertices)[1].color);
        (*vertices)[2].color = ComputeVertexShadingColor((*vertices)[2].color);

        break;

    /* --------------------------------------------------------------------- GOURAUD */
    case LIGHTMODEL::GOURAUD :

        for (unsigned int n = 0; n < vertices->size(); n++)
        {
            N = (*vertices)[n].normal;
            N.normalize();

            vec4 vertexPos = (*vertices)[n].pos_WS;
            vertexPos.w = 0;

            V = _eyepoint - vertexPos;
            V.normalize();

            for (unsigned int i  = 0; i < _lightSources.size(); i++)
            {
                vec4 L = _lightSources[i].position - vertexPos;
                double dist = L.length();
                _lightSources[i].f_att =  1.0 / (_lightSources[i].A + _lightSources[i].B * dist);
                L.normalize();
                vec4 R;
                R = (N * 2.0f * (N.dot(L))) - L;
                R.normalize();

                _lightSources[i].diffuse = N.dot(L);

				// Clamp V.R if < 0
				double VdotR = V.dot(R);
				if (VdotR < 0.0) { VdotR = 0.0; }

                _lightSources[i].specular = _k_s * pow(V.dot(R), _p);
            }
            (*vertices)[n].color = ComputeVertexShadingColor((*vertices)[n].color);
        }

        break;

    /* ----------------------------------------------------------------------- PHONG */
    case LIGHTMODEL::PHONG :

        // Nothing to intialize

        break;
    }
}


Color Lighting::PerformLightingCalculation(Color objectColor, vec4& N, vec4& vertexPos)
{
	if (!doLighting) { return objectColor; }

    Color_f object_fl = objectColor.convertTodouble();
    Color_f result = object_fl;
    result = result * _ambient;

    vec4 V;

    switch (_model)
    {
    case LIGHTMODEL::FLAT :
        return objectColor;
        break;

    case LIGHTMODEL::GOURAUD :
        return objectColor;
        break;

    case LIGHTMODEL::PHONG :

        N.normalize();

        vertexPos.w = 0;

        V = _eyepoint - vertexPos;
        V.normalize();

        for (unsigned int i  = 0; i < _lightSources.size(); i++)
        {
            vec4 L = _lightSources[i].position - vertexPos;
            double dist = L.length();
            _lightSources[i].f_att =  1.0 / (_lightSources[i].A + _lightSources[i].B * dist);
            L.normalize();
            vec4 R;
            R = (N * 2.0f * (N.dot(L))) - L;
            R.normalize();

            _lightSources[i].diffuse = N.dot(L);

			// Clamp V.R if < 0
			double VdotR = V.dot(R);
			if (VdotR < 0.0) { VdotR = 0.0; }

            _lightSources[i].specular = _k_s * pow(V.dot(R), _p);
        }

        return ComputeVertexShadingColor(objectColor);

        break;

    default :
        return objectColor;
    }
}





