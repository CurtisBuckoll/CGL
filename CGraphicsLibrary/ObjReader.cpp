#include "ObjReader.h"
#include "utility.h"

#include <iostream>


/* SimpReader helper functions ----------------------------------------------------- */


// Report error and exit program
void ReportSytaxErrorObj(std::vector<std::string> tokens, std::string errorMsg = "")
{
    std::cout << "Encountered syntax error in .obj script in the following line:" << std::endl;
    for (unsigned int i = 0; i < tokens.size(); i++)
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


/*  Print array info  */
void ObjReader::PrintArrayInfo()
{
    std::cout << std::endl;
    for (unsigned int i = 0; i < _normals->size(); i++)
    {
        (*_normals)[i].print();
    }
    std::cout << std::endl;
    for (unsigned int i = 0; i < _vertices->size(); i++)
    {
        (*_vertices)[i].pos.print();
    }

    // print face reading
    for (unsigned int k = 0; k < _faces->size(); k++)
    {
        for (unsigned int j = 0; j < (*_faces)[k].vertices.size(); j++)
        {
             std::cout << (*_faces)[k].vertices[j].vIndex << " "
                       << (*_faces)[k].vertices[j].nIndex << std::endl;
        }
        std::cout << std::endl;
    }
}


/* End helper functions ------------------------------------------------------------ */


ObjReader::ObjReader(const std::string& filepath, std::vector<Vertex>* vertices,
                     std::vector<vec4>* normals, std::vector<face>* faces,
                     Color surfaceColor) :
    _surfaceColor(surfaceColor)
{
    _currentFile.open(filepath);
    if (!_currentFile.is_open())
    {
        std::cout << "Failed to open file upon initialization of ObjReader object. Exiting." << std::endl;
        std::cout << "Filepath: " + filepath << std::endl;
        exit(-1);
    }

    _vertices = vertices;
    _normals = normals;
    _faces = faces;

    // Append 'empty' vertices to index 0 of _vertices and _normals
    _vertices->push_back(Vertex(vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), Color()));
    _normals->push_back(vec4(0.0, 0.0, 0.0, 0.0));
}


ObjReader::~ObjReader()
{
    // Empty
}


int ObjReader::absoluteIndex_vertex(int index)
{
    if (index >= 0)
    {
        return index;
    }
    return (int)_vertices->size() + index;
}


int ObjReader::absoluteIndex_normal(int index)
{
    if (index >= 0)
    {
        return index;
    }
    return (int)_normals->size() + index;
}


face ObjReader::splitFaceInstr(const std::vector<std::string>& tokens)
{
    face currFace;
    for (unsigned int i = 1; i < tokens.size(); i++)
    {
        vDescrip currVertex;
        std::string value = "";
        int C = 0;

        // extract vertex index
        while (tokens[i][C] != '/' && tokens[i][C] != '\0')
        {
            value += tokens[i][C];
            C++;
        }
        currVertex.vIndex = absoluteIndex_vertex(stoi(value));
        if (tokens[i][C] == '\0')
        {
            currVertex.nIndex = 0;
            currFace.vertices.push_back(currVertex);
            continue;
        }

        value = "";
        C++;

        // scan texture index and ignore
        while (tokens[i][C] != '/' && tokens[i][C] != '\0')
        {
            C++;
        }
        if (tokens[i][C] == '\0')
        {
            currVertex.nIndex = 0;
            currFace.vertices.push_back(currVertex);
            continue;
        }

        C++;

        // extract normal index
        while (tokens[i][C] != '/' && tokens[i][C] != '\0')
        {
            value += tokens[i][C];
            C++;
        }
        if (!value.empty())
        {
            currVertex.nIndex = absoluteIndex_normal(stoi(value));
        }
        else
        {
            currVertex.nIndex = 0;
        }
        currFace.vertices.push_back(currVertex);
    }
    return currFace;
}


void ObjReader::InterpretLineObj(const std::vector<std::string>& tokens)
{
    if (tokens[0][0] == '#')
    {
        return;
    }
    else if (tokens[0] == "v")
    {
        if (tokens.size() == 4)
        {
            vec4 vertex = vec4(stod(tokens[1]), stod(tokens[2]), -1.0 * stod(tokens[3]));

            _vertices->push_back(Vertex(vertex, vertex, vec4(0.0, 0.0, 0.0, 0.0), _surfaceColor));
        }
        else if (tokens.size() == 5)
        {
            vec4 vertex = vec4(stod(tokens[1]), stod(tokens[2]), -1.0 * stod(tokens[3]), stod(tokens[4]));

            _vertices->push_back(Vertex(vertex, vertex, vec4(0.0, 0.0, 0.0, 0.0), _surfaceColor));
        }
        else if (tokens.size() == 7)
        {
            vec4 vertex = vec4(stod(tokens[1]), stod(tokens[2]), -1.0 * stod(tokens[3]));

            Color vColor = Color((unsigned char)(stod(tokens[4]) * 255),
                                 (unsigned char)(stod(tokens[5]) * 255),
                                 (unsigned char)(stod(tokens[6]) * 255));
            _vertices->push_back(Vertex(vertex, vertex, vec4(0.0, 0.0, 0.0, 0.0), vColor));

        }
        else if (tokens.size() == 8)
        {
            vec4 vertex = vec4(stod(tokens[1]), stod(tokens[2]), -1.0 * stod(tokens[3]), stod(tokens[4]));

            Color vColor = Color((unsigned char)(stod(tokens[5]) * 255),
                                 (unsigned char)(stod(tokens[6]) * 255),
                                 (unsigned char)(stod(tokens[7]) * 255));
            _vertices->push_back(Vertex(vertex, vertex, vec4(0.0, 0.0, 0.0, 0.0), vColor));
        }
    }
    else if (tokens[0] == "vn")
    {
        if (tokens.size() == 4)
        {
            _normals->push_back(vec4(stod(tokens[1]), stod(tokens[2]), -1.0 * stod(tokens[3]), 0.0));
        }
        else
        {
            ReportSytaxErrorObj(tokens);
        }
    }
    else if (tokens[0] == "f")
    {
        face Face = splitFaceInstr(tokens);

        // split non triangular polygons into triangles.
        if (Face.vertices.size() > 3)
        {
            for (unsigned int i = 1; i < Face.vertices.size() - 1; i++)
            {
                face triangle;
                triangle.vertices.push_back(Face.vertices[0]);
                triangle.vertices.push_back(Face.vertices[i]);
                triangle.vertices.push_back(Face.vertices[i+1]);
                _faces->push_back(triangle);
            }
        }
        else
        {
            _faces->push_back(Face);
        }
    }
}


/*	Splits an std::string into individual symobls and stores in a vector array.
 */
void splitStringObj(const std::string& currLine, std::vector<std::string>* tokens)
{
    std::string word;

    for (unsigned int i = 0; i < currLine.size(); i++)
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
}


void ObjReader::Read()
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

        splitStringObj(currLine, &tokens);
        if (!tokens.empty())
        {
            InterpretLineObj(tokens);
        }

        tokens.clear();
    }

    _currentFile.close();
}
