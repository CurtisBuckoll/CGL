#include "SimpReader.h"
#include "Point.h"
#include "Polygon.h"
#include "ObjReader.h"
#include "utility.h"

#include <iostream>


/* SimpReader helper functions ----------------------------------------------------- */

/** Performs transformations by CTM, CAMERA, and PROJ
 *  Stores the world space coord in WS_Coord, and Screen space coord in SS_Coord
 */
/*
void SimpReader::transformVertices(vec4 vertex, vec4* WS_Coord, vec4* SS_Coord)
{
    vertex.hgDivision();

    vec4 wsc, ssc;

    wsc = _CTM * vertex;
    ssc = _PROJ * _CAMERA * wsc;
    if (ssc.z != 0 )
    {
        ssc.x = ssc.x / ssc.z;
        ssc.y = ssc.y / ssc.z;
    }
    ssc.w = 1.0f;
    scaleToScreen(&ssc);
    ssc = _SCREEN * ssc;

    *WS_Coord = wsc;
    *SS_Coord = ssc;
}
*/


void SimpReader::TransformToScreen(const DynamicArray<Vertex>& vertices, std::vector<std::vector<Vertex>>* transformed)
{
    std::vector<Vertex> vertexList;

    for (int i = 0; i < vertices.length(); i++)
    {
        Vertex newVertex = vertices[i];
        newVertex.pos.hgDivision();
        newVertex.pos_WS = _CTM * newVertex.pos;

		//newVertex.pos_WS.print();

        newVertex.pos = _CAMERA * newVertex.pos_WS;

        //newVertex.normal.print();

        // Transform the normal with respect to WSC, if a nonzero normal provided
        if (newVertex.normal != vec4(0.0f, 0.0f, 0.0f, 0.0f))
        {
            newVertex.normal = newVertex.normal * _invCTM;
            newVertex.normal.w = 0.0f;  /// DO WE DO THIS?!
            newVertex.normal.normalize();
        }

        //newVertex.normal.print();

        vertexList.push_back(newVertex);
    }


    // Compute default face normal - behaviour will set line/point normals to 0.
    vec4 faceNormal = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    if (vertexList.size() >= 3)
    {
        faceNormal = cross(vertexList[2].pos_WS, vertexList[1].pos_WS, vertexList[0].pos_WS);
    }
    // Provide a default face normal to a vertex if one is not specified
    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].normal == vec4(0.0f, 0.0f, 0.0f, 0.0f))
        {
            vertexList[i].normal = faceNormal;
            vertexList[i].normal.w = 0.0f;
            vertexList[i].normal.normalize();
            //std::cout << "default normal provided" << std::endl;
        }
        //vertexList[i].normal.print();
    }


    // Print the dot product: remove later
    //for (int j = 0; j < vertexList.size(); j++)
    //{
        //std::cout << vertexList[j].normal.dot(vertexList[j].pos_WS -
        //                                       vertexList[(j+1)% vertexList.size()].pos_WS) << std::endl;
    //}

/*
    for (int i = 0 ; i < vertexList.size(); i++)
    {
        vertexList[i].pos.print();
    }
    std::cout << std::endl;
*/

    // Perform polygon culling/clipping
    if (_frustum.cullToFrustum(vertexList))
    {
        return;
    }
    _frustum.clipToFrustum(&vertexList);

/*
    for (int i = 0 ; i < vertexList.size(); i++)
    {
        vertexList[i].pos.print();
    }
    std::cout << std::endl;
*/


    // Triangulate cipped vertices (if necessary)
    std::vector<std::vector<Vertex>> splitVertexList;
    if (vertexList.size() >= 3)
    {
        for (int k = 1; k < vertexList.size() - 1; k++)
        {
            std::vector<Vertex> triangle;
            triangle.push_back(vertexList[0]);
            triangle.push_back(vertexList[k]);
            triangle.push_back(vertexList[k+1]);
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
            if (splitVertexList[j][k].pos.z != 0 )
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


void SimpReader::scaleToScreen(vec4* vertex)
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


Color SimpReader::computeAmbientLight(Color color)
{
    return Color((unsigned char)(color.r * _ambientColor.r),
                 (unsigned char)(color.g * _ambientColor.g),
                 (unsigned char)(color.b * _ambientColor.b));
}


// Report error and exit program
void ReportSytaxError(std::vector<std::string> tokens, std::string errorMsg = "")
{
    std::cout << "Encountered syntax error in .simp script in the following line:" << std::endl;
    for (int i = 0; i < tokens.size(); i++)
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


SimpReader::SimpReader(std::string filepath, Window* window,
                       zBuffer* buffer, Lighting* _LightEngine,
                       mat4 CTM, mat4 CAMERA, mat4 SCREEN,
                       float depthNear, float depthFar,
                       Color_f ambientColor, Color depthColor, Color surfaceColor,
                       bool wireFrame) :
    _wireFrame(wireFrame),
    _CTM(CTM),
    _windowSpaceMultiplier(3.25f),
    _depthNear(depthNear),
    _depthFar(depthFar),
    _ambientColor(ambientColor),
    _depthColor(depthColor),
    _surfaceColor(surfaceColor),
    _CAMERA(CAMERA),
    _PROJ(MAT_TYPE::PROJECTION),
    _SCREEN(SCREEN)
{
    _currentFile.open(filepath);
    if (!_currentFile.is_open())
    {
        std::cout << "Failed to open file upon initialization of SimpReader object. Exiting." << std::endl;
        std::cout << "Filepath: " + filepath << std::endl;
        exit(-1);
    }
    _window = window;
    _zBuffer = buffer;
    _lightEngine = _LightEngine;

    _frustum = Clip();
    _frustum.Init(buffer->xLo, buffer->yLo, buffer->xHi, buffer->yHi, buffer->hither, buffer->yon);
}


SimpReader::~SimpReader()
{
    // Empty
}


void SimpReader::Interpret(const std::vector<std::string>& tokens)
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
            ReportSytaxError(tokens);
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
            ReportSytaxError(tokens);
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
            ReportSytaxError(tokens);
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
            ReportSytaxError(tokens);
        }

        std::vector<std::vector<Vertex>> transformedVertices;

        TransformToScreen(vertices, &transformedVertices);

        if (transformedVertices.size() == 2)
        {
            Line::DDA(transformedVertices[0][0], transformedVertices[0][1], _zBuffer, _window);
        }

        /*
        DynamicArray<Vertex> vertexList;
        for (int i = 0; i < 2; i++)
        {
            vec4 WSC;
            vec4 SSC;
            vec4 result = points[i];
            transformVertices(result, &WSC, &SSC);

            Color vertexColor = computeAmbientLight(unpackColor(points[i].color));
            Vertex vertex = Vertex(SSC, WSC, vec4(0.0f, 0.0f, 0.0f, 0.0f), vertexColor);
            vertexList.append(vertex);
        }

        Line::DDA(vertexList[0], vertexList[1], _zBuffer, _drawable);
        */
    }
/* ------------------------------------------------------------------------- POLYGON */
    else if (tokens[0] == "polygon")
    {
        DynamicArray<Vertex> vertices;
        if (tokens.size() == 10)
        {
            for (int i = 1; i < tokens.size(); i += 3)
            {
                vec4 position = vec4(stof(tokens[i]), stof(tokens[i + 1]), stof(tokens[i + 2]));
                Vertex vertex = Vertex(position, position, vec4(0.0f, 0.0f, 0.0f, 0.0f), _surfaceColor);
                vertices.append(vertex);
            }
        }
        else if (tokens.size() == 19)
        {
            for (int i = 1; i < tokens.size(); i += 6)
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
            ReportSytaxError(tokens, "Incorrect polygon parameterization");
        }

        std::vector<std::vector<Vertex>> transformedVertices;

        TransformToScreen(vertices, &transformedVertices);

        for (int j = 0; j < transformedVertices.size(); j++)
        {
            _lightEngine->init(&transformedVertices[j]);
            Polygon::drawPolygonLERP(transformedVertices[j], 1.0f, _wireFrame, _zBuffer, _window, _lightEngine);
        }
        /*
        DynamicArray<Vertex> vertexList;
        for (int i = 0; i < points.length(); i++)
        {
            vec4 WSC;
            vec4 SSC;
            vec4 result = points[i];
            transformVertices(result, &WSC, &SSC);


            //** compute and transform the normal here!


            Color vertexColor = computeAmbientLight(unpackColor(result.color));
            Vertex vertex = Vertex(SSC, WSC, vec4(0.0f, 0.0f, 0.0f, 0.0f), vertexColor);
            vertexList.append(vertex);
        }

        Polygon::drawPolygonLERP(vertexList, 1.0f, _wireFrame, _zBuffer, _drawable);
        */
    }
/* ---------------------------------------------------------------------------- FILE */
    else if (tokens[0] == "file")
    {
        if (tokens.size() == 2)
        {
            SimpReader fileInclude("./" + tokens[1] + ".simp", _window,
                                   _zBuffer, _lightEngine, _CTM, _CAMERA, _SCREEN, _depthNear, _depthFar,
                                   _ambientColor, _depthColor, _surfaceColor, _wireFrame);
            simpArgs modifiedVals = fileInclude.Read();


            // Set any parameters modifed by the simp file
            _ambientColor = modifiedVals.ambientColor;
            _CAMERA = modifiedVals.CAMERA;
            _CTM = modifiedVals.CTM;
            _depthColor = modifiedVals.depthColor;
            _depthFar = modifiedVals.depthFar;
            _depthNear = modifiedVals.depthNear;
            _surfaceColor = modifiedVals.surfaceColor;
            _wireFrame = modifiedVals.wireFrame;

            // Update other necessary parameters
            _zBuffer->depthNear = _depthNear;
            _zBuffer->depthFar = _depthFar;
            _zBuffer->depthColor = _depthColor;

        }
        else
        {
            ReportSytaxError(tokens);
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
            ReportSytaxError(tokens);
        }
    }
/* -------------------------------------------------------------------------- CAMERA */
    else if (tokens[0] == "camera")
    {
        if (tokens.size() == 7)
        {
            _CAMERA = _CTM.inverse();

            // Get eyepoint for lighting calculation
            vec4 eyePoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            eyePoint = _CTM * eyePoint;
            _lightEngine->setEyePoint(eyePoint);

            // Update zBuffer
            delete _zBuffer;
            _zBuffer = new zBuffer(650, 650, stof(tokens[5]), stof(tokens[6]),
                                   stof(tokens[1]), stof(tokens[2]),
                                   stof(tokens[3]), stof(tokens[4]));
            _zBuffer->depthNear = _depthNear;
            _zBuffer->depthFar = _depthFar;
            _zBuffer->depthColor = _depthColor;
            _frustum.Init(_zBuffer->xLo, _zBuffer->yLo, _zBuffer->xHi, _zBuffer->yHi, _zBuffer->hither, _zBuffer->yon);
            _SCREEN.translate(((float)_zBuffer->width / 2), ((float)_zBuffer->height / 2), 0.0f);
        }
        else
        {
            ReportSytaxError(tokens);
        }
    }
/* ----------------------------------------------------------------------------- OBJ */
    else if (tokens[0] == "obj")
    {
        std::vector<Vertex> vertices;
        std::vector<vec4> normals;
        std::vector<face> faces;
        _invCTM = _CTM.inverse(); // This is here for efficiency, only need to compute once.

        // Populate the lists with .obj data
        ObjReader object("./" + tokens[1] + ".obj", &vertices, &normals, &faces, _surfaceColor);
        object.Read();

        // Render the data
        for (int i = 0; i < faces.size(); i++)
        {
            DynamicArray<Vertex> vertexList;
            for (int k = 0; k < faces[i].vertices.size(); k++)
            {
                Vertex currVertex = vertices[faces[i].vertices[k].vIndex];
                currVertex.normal = normals[faces[i].vertices[k].nIndex];
                vertexList.append(currVertex);

				//currVertex.pos.print();
            }
            std::vector<std::vector<Vertex>> transformedVertices;

            TransformToScreen(vertexList, &transformedVertices);

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
    }
/* ------------------------------------------------------------------------- AMBIENT */
    else if (tokens[0] == "ambient")
    {
        /*
        _ambientColor = Color((unsigned char)(255 * stof(tokens[1])),
                              (unsigned char)(255 * stof(tokens[2])),
                              (unsigned char)(255 * stof(tokens[3])));
                              */
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
        _zBuffer->depthNear = _depthNear;
        _zBuffer->depthFar = _depthFar;
        _zBuffer->depthColor = _depthColor;
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
            ReportSytaxError(tokens);
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
            ReportSytaxError(tokens);
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
        ReportSytaxError(tokens, "Error: Invalid command" );
    }
}


/*	Splits an std::string into individual symobls and stores in a vector array.
 *	Strips away characters '(', ',' and ')'.
 */
void splitString(const std::string& currLine, std::vector<std::string>* tokens)
{
    std::string word;

    for (int i = 0; i < currLine.size(); i++)
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
    for (int i = 0; i < tokens->size(); i++)
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


simpArgs SimpReader::Read()
{
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
        for (int i = 0; i < currLine.size(); i++)
        {
            if ( currLine[i] == ',')
            {
                currLine[i] = ' ';
            }
        }

        // Read and interpret line
        splitString(currLine, &tokens);
        if (!tokens.empty())
        {
            Interpret(tokens);
        }

        tokens.clear();
    }
    _currentFile.close();

    return simpArgs(_CTM, _CAMERA, _wireFrame, _depthNear, _depthFar, _ambientColor, _depthColor, _surfaceColor);
}
