#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Matrix.h"
#include "Vertex.h"

struct vDescrip {
    int vIndex;
    int nIndex;
};

struct face {
   std::vector<vDescrip> vertices;
};

class ObjReader
{
public:
    ObjReader(const std::string& filepath, std::vector<Vertex>* vertices,
              std::vector<vec4>* normals, std::vector<face>* faces,
              Color surfaceColor);
    ~ObjReader();

    void Read();

private:
    std::ifstream _currentFile;
    std::vector<Vertex>* _vertices;
    std::vector<vec4>* _normals;
    std::vector<face>* _faces;

    Color _surfaceColor;

    void PrintArrayInfo();
    int absoluteIndex_vertex(int index);
    int absoluteIndex_normal(int index);
    face splitFaceInstr(const std::vector<std::string>& tokens);
    void InterpretLineObj(const std::vector<std::string>& tokens);
};
