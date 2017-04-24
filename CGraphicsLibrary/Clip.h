#pragma once

#include "Matrix.h"
#include "DynamicArray.h"
#include "Vertex.h"

#include <vector>


struct FrustumParams {
	float hither;
	float yon;
	float xLo;
	float xHi;
	float yLo;
	float yHi;

	FrustumParams()
	{
		hither = yon = xLo = xHi = yLo = yHi = 0.0f;
	}

	FrustumParams(float Hither, float Yon, float XLo, float XHi, float YLo, float YHi) :
		hither(Hither),
		yon(Yon),
		xLo(XLo),
		xHi(XHi),
		yLo(YLo),
		yHi(YHi)
	{
		// Empty
	}
};

const int NUM_PLANES = 6;

class Clip {
private:
    vec4 _frustum[NUM_PLANES];

	inline bool cullPlane(const std::vector<Vertex>& vertices, const vec4& plane);
	inline void clipPlane(std::vector<Vertex>* vertices, const vec4& plane);

public:
	FrustumParams dimensions;

    Clip();
    ~Clip();

	void Init(float xLo, float yLo, float xHi, float yHi, float hither, float yon);
    bool cullToFrustum(const std::vector<Vertex>& vertices);
    void clipToFrustum(std::vector<Vertex>* vertices);
};
