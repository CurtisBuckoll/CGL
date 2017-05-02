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

	FrustumParams(float Hither, float Yon, float XLo, float YLo, float XHi, float YHi) :
		hither(Hither),
		yon(Yon),
		xLo(XLo),
		yLo(YLo),
		xHi(XHi),
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
	FrustumParams parameters;

    Clip();
    ~Clip();

	void Init(FrustumParams params);
    bool cullToFrustum(const std::vector<Vertex>& vertices);
    void clipToFrustum(std::vector<Vertex>* vertices);
	bool backFaceCull(const std::vector<Vertex>& vertices);
};
