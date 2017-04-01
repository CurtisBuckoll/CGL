#pragma once

#include "Matrix.h"
#include "DynamicArray.h"
#include "Vertex.h"

#include <vector>

const int NUM_PLANES = 6;

class Clip {
private:
    vec4 _frustum[NUM_PLANES];

    //Color computeClippedColour(Vertex p0, Vertex p1, float t);

public:
    Clip();
    ~Clip();

    void Init (float xLo, float yLo, float xHi, float yHi, float hither, float yon);

    bool cullToFrustum(const std::vector<Vertex>& vertices);
    bool cullPlane(const std::vector<Vertex>& vertices, const vec4& plane);

    void clipToFrustum(std::vector<Vertex>* vertices);
    void clipPlane(std::vector<Vertex>* vertices, const vec4& plane);
};
