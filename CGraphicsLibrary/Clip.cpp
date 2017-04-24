#include "Clip.h"
#include "utility.h"

Clip::Clip()
{
    for(int i = 0; i < 6; i++)
    {
        _frustum[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

Clip::~Clip()
{
    // Empty
}

void Clip::Init(FrustumParams params)
{
	parameters = params;

    vec4 top_L = vec4(params.xLo, params.yHi, 1.0f, 0.0f);
    vec4 bot_L = vec4(params.xLo, params.yLo, 1.0f, 0.0f);
    vec4 bot_R = vec4(params.xHi, params.yLo, 1.0f, 0.0f);
    vec4 top_R = vec4(params.xHi, params.yHi, 1.0f, 0.0f);

    // Build the clipping planes
    _frustum[0] = vec4(0.0f, 0.0f, 1.0, -params.hither);	// Front
    _frustum[1] = vec4(0.0f, 0.0f, -1.0, params.yon);		// Back
    _frustum[2] = top_L.cross(bot_L);						// Left
    _frustum[3] = bot_L.cross(bot_R);						// Bottom
    _frustum[4] = bot_R.cross(top_R);						// Right
    _frustum[5] = top_R.cross(top_L);						// Top
}

void Clip::clipToFrustum(std::vector<Vertex>* vertices)
{
    clipPlane(vertices, _frustum[0]);
    clipPlane(vertices, _frustum[1]);
    clipPlane(vertices, _frustum[2]);
    clipPlane(vertices, _frustum[3]);
    clipPlane(vertices, _frustum[4]);
    clipPlane(vertices, _frustum[5]);
}

bool Clip::cullToFrustum(const std::vector<Vertex>& vertices)
{
    for (int i = 0; i < 6; i++)
    {
        if (cullPlane(vertices, _frustum[i]))
        {
            return true;
        }
    }
    return false;
}

inline bool Clip::cullPlane(const std::vector<Vertex>& vertices, const vec4& plane)
{
    int count = 0;
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        if (plane.dot(vertices[i].pos) < 0)
        {
            count++;
        }
    }
    if (count == vertices.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}


inline Color computeClippedColour(const Vertex& p0, const Vertex& p1, float t)
{
    unsigned char clippedColor_R = (unsigned char)(t * p0.color.r + (1-t) * p1.color.r);
    unsigned char clippedColor_G = (unsigned char)(t * p0.color.g + (1-t) * p1.color.g);
    unsigned char clippedColor_B = (unsigned char)(t * p0.color.b + (1-t) * p1.color.b);

    return Color(clippedColor_R, clippedColor_G, clippedColor_B);
}


inline vec4 computeClippedNormal(const Vertex& p0, const Vertex& p1, float t)
{
    return (p0.normal * t) + (p1.normal * (1-t));
}


inline vec4 computeClippedWSC(const Vertex& p0, const Vertex& p1, float t)
{
    return (p0.pos_WS * t) + (p1.pos_WS * (1-t));
}


void Clip::clipPlane(std::vector<Vertex>* vertices, const vec4& plane)
{
    if (vertices->size() < 2)
        { return; }


    // Find first vertex in +ve halfspace
    bool inPositiveHS = true;
    int startIndex = 0;
    for (unsigned int i = 0; i < vertices->size(); i++)
    {
        if (plane.dot((*vertices)[i].pos) > 0)
        {
            startIndex = i;
            break;
        }
    }

    DynamicArray<Vertex> clippedVertices;

    // Use plane_D0 for denominator for calculation
    vec4 plane_D0 = plane;
    plane_D0.w = 0;


    // Corner case: we are clipping a line. We handle seperately. We know we are starting in +ve halfspace.
    if (vertices->size() == 2)
    {
        Vertex prev = (*vertices)[startIndex];
        Vertex curr = (*vertices)[(startIndex + 1) % vertices->size()];

        clippedVertices.append(prev);

        Vertex clippedVec = curr;
        if (plane.dot(curr.pos) < 0)
        {
            float t = -(curr.pos.dot(plane) / (prev.pos.dot(plane_D0) - curr.pos.dot(plane_D0)));
            clippedVec.pos = vec4(t * prev.pos.x + (1 - t) * (curr.pos.x),
                                  t * prev.pos.y + (1 - t) * (curr.pos.y),
                                  t * prev.pos.z + (1 - t) * (curr.pos.z));

            clippedVec.pos_WS = computeClippedWSC(prev, curr, t);
            clippedVec.color = computeClippedColour(prev, curr, t);
        }

        clippedVertices.append(clippedVec);

        vertices->clear();
        for (int i = 0; i < clippedVertices.length(); i++)
        {
            vertices->push_back(clippedVertices[i]);
        }
        return;
    }


    // Default case: we are clipping a polygon.
    for (unsigned int i = 0; i < vertices->size(); i++)
    {
        Vertex prev = (*vertices)[(startIndex + i) % vertices->size()];
        Vertex curr = (*vertices)[(startIndex + i + 1) % vertices->size()];

        Vertex clippedVec = curr;

        // Do not clip curr
        if (plane.dot(curr.pos) >= 0 && inPositiveHS)
        {
            clippedVertices.append(clippedVec);
            continue;
        }
        // We have crossed over from +ve to -ve halfspace
        else if (plane.dot(curr.pos) < 0 && inPositiveHS)
        {
            // Perform intersection calculation: "curr" is the vertex of interest
            float t = -(curr.pos.dot(plane) / (prev.pos.dot(plane_D0) - curr.pos.dot(plane_D0)));
            clippedVec.pos = vec4(t * prev.pos.x + (1 - t) * (curr.pos.x),
                                  t * prev.pos.y + (1 - t) * (curr.pos.y),
                                  t * prev.pos.z + (1 - t) * (curr.pos.z));

            clippedVec.pos_WS = computeClippedWSC(prev, curr, t);
            clippedVec.normal = computeClippedNormal(prev, curr, t);

            clippedVec.color = computeClippedColour(prev, curr, t);
            clippedVertices.append(clippedVec);
            inPositiveHS = !inPositiveHS;
        }
        // We have crossed over from -ve to +ve halfspace
        else if ((plane.dot(curr.pos) > 0 && !inPositiveHS))
        {
            float t = -(curr.pos.dot(plane) / (prev.pos.dot(plane_D0) - curr.pos.dot(plane_D0)));
            clippedVec.pos = vec4(t * prev.pos.x + (1 - t) * (curr.pos.x),
                                  t * prev.pos.y + (1 - t) * (curr.pos.y),
                                  t * prev.pos.z + (1 - t) * (curr.pos.z));

            clippedVec.pos_WS = computeClippedWSC(prev, curr, t);
            clippedVec.normal = computeClippedNormal(prev, curr, t);

            clippedVec.color = computeClippedColour(prev, curr, t);
            clippedVertices.append(clippedVec);
            clippedVertices.append(curr);
            inPositiveHS = !inPositiveHS;
        }
        // Else we ignore vertices in -ve halfspace
    }

    vertices->clear();
    for (int i = 0; i < clippedVertices.length(); i++)
    {
        vertices->push_back(clippedVertices[i]);
    }
}
