#pragma once

#include "Matrix.h"
#include "DynamicArray.h"
#include "Vertex.h"

#include <vector>


struct FrustumParams {
	double hither;
	double yon;
	double xLo;
	double xHi;
	double yLo;
	double yHi;

	FrustumParams()
	{
		hither = yon = xLo = xHi = yLo = yHi = 0.0;
	}

	FrustumParams(double Hither, double Yon, double XLo, double YLo, double XHi, double YHi) :
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
