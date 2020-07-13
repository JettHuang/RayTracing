// standard perlin noise.
// https://adrianb.io/2014/08/09/perlinnoise.html
//

#pragma once

#include "basic.h"
#include "vec3.h"


// perlin 3d
class FPerlin3D
{
public:
	FPerlin3D();
	~FPerlin3D();

	double noise(const FPoint3& p) const;
	double octaveNoise(const FPoint3& p, int octaves, double persistence) const;

protected:
	double fade(double t) const;
	int inc(int num) const;
	double grad(int hash, double x, double y, double z) const;

protected:
	int* perm;
	int repeat;
};

