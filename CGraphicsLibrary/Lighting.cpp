#include "Lighting.h"

#include "utility.h"

#include <iostream>


/* LightSource Methods ------------------------------------------------------------- */


LightSource::LightSource()
{
    position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    A = 0.0f;
    B = 0.0f;
    I = Color_f();
}


LightSource::LightSource(const vec4& pos, float A, float B, const Color_f& intensity)
{
    position = pos;
    position.hgDivision();
    position.w = 0.0f;
    this->A = A;
    this->B = B;
    I = intensity;

    diffuse = 1.0f;
    specular = 1.0f;
    f_att = 1.0f;
}


LightSource::~LightSource()
{
    //Empty
}


Color Lighting::ComputeVertexShadingColor(Color objectColor)
{
    Color_f object_fl = objectColor.convertToFloat();
    Color_f result = object_fl;
    result = result * _ambient;

    for (int i = 0; i < _lightSources.size(); i++)
    {
        // Light does not hit face if 0
        if (_lightSources[i].diffuse <= 0) { continue; }

        // R
        float I_r = _lightSources[i].I.r;
        float k_r = object_fl.r;
        float R = I_r * _lightSources[i].f_att * (k_r * _lightSources[i].diffuse + _lightSources[i].specular);
        // G
        float I_g = _lightSources[i].I.g;
        float k_g = object_fl.g;
        float G = I_g * _lightSources[i].f_att * (k_g * _lightSources[i].diffuse + _lightSources[i].specular);
        // B
        float I_b = _lightSources[i].I.b;
        float k_b = object_fl.b;
        float B = I_b * _lightSources[i].f_att * (k_b * _lightSources[i].diffuse + _lightSources[i].specular);

        result.r += R;
        result.g += G;
        result.b += B;
    }

    if (result.r > 1.0f) {result.r = 1.0f;}
    if (result.g > 1.0f) {result.g = 1.0f;}
    if (result.b > 1.0f) {result.b = 1.0f;}

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
    _ambient = Color_f(0.0f, 0.0f, 0.0f);
}


Lighting::~Lighting()
{
    _lightSources.clear();
}


void Lighting::setSurfaceProperties(float K_s, int P)
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
    _eyepoint.w = 0.0f;
}


void Lighting::addLightSource(const vec4& pos, float A, float B, const Color_f& intensity)
{
    _lightSources.push_back(LightSource(pos, A, B, intensity));
}


void Lighting::init(std::vector<Vertex>* vertices)
{
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
        centerPos = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        N = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < vertices->size(); i++)
        {
            centerPos = centerPos + (*vertices)[i].pos_WS;
            N = N + (*vertices)[i].normal;
        }
        centerPos = centerPos * (1.0f / 3);
        centerPos.w = 0;
        N = N * (1.0f / 3);
        N.normalize();

        V = _eyepoint - centerPos;
        V.normalize();

        for (int i  = 0; i < _lightSources.size(); i++)
        {
            vec4 L = _lightSources[i].position - centerPos;
            float dist = L.length();
            _lightSources[i].f_att =  1.0f / (_lightSources[i].A + _lightSources[i].B * dist);
            L.normalize();
            vec4 R;
            R = (N * 2.0f * (N.dot(L))) - L;
            R.normalize();

            _lightSources[i].diffuse = N.dot(L);
            _lightSources[i].specular = _k_s * pow(V.dot(R), _p);
        }

        (*vertices)[0].color = ComputeVertexShadingColor((*vertices)[0].color);
        (*vertices)[1].color = ComputeVertexShadingColor((*vertices)[1].color);
        (*vertices)[2].color = ComputeVertexShadingColor((*vertices)[2].color);

        break;

    /* --------------------------------------------------------------------- GOURAUD */
    case LIGHTMODEL::GOURAUD :

        for (int n = 0; n < vertices->size(); n++)
        {
            N = (*vertices)[n].normal;
            N.normalize();

            vec4 vertexPos = (*vertices)[n].pos_WS;
            vertexPos.w = 0;

            V = _eyepoint - vertexPos;
            V.normalize();


            for (int i  = 0; i < _lightSources.size(); i++)
            {
                vec4 L = _lightSources[i].position - vertexPos;
                float dist = L.length();
                _lightSources[i].f_att =  1.0f / (_lightSources[i].A + _lightSources[i].B * dist);
                L.normalize();
                vec4 R;
                R = (N * 2.0f * (N.dot(L))) - L;
                R.normalize();

                _lightSources[i].diffuse = N.dot(L);
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


Color Lighting::PerformLightingCalculation(Color objectColor, vec4 N, vec4 vertexPos)
{
    Color_f object_fl = objectColor.convertToFloat();
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

        for (int i  = 0; i < _lightSources.size(); i++)
        {
            vec4 L = _lightSources[i].position - vertexPos;
            float dist = L.length();
            _lightSources[i].f_att =  1.0f / (_lightSources[i].A + _lightSources[i].B * dist);
            L.normalize();
            vec4 R;
            R = (N * 2.0f * (N.dot(L))) - L;
            R.normalize();

            _lightSources[i].diffuse = N.dot(L);
            _lightSources[i].specular = _k_s * pow(V.dot(R), _p);
        }

        return ComputeVertexShadingColor(objectColor);

        break;

    default :
        return objectColor;
    }


    /*
    if (result.r > 1.0f) {result.r = 1.0f;}
    if (result.g > 1.0f) {result.g = 1.0f;}
    if (result.b > 1.0f) {result.b = 1.0f;}

    return Color((unsigned char)(255 * result.r),
                 (unsigned char)(255 * result.g),
                 (unsigned char)(255 * result.b));
     */
}





