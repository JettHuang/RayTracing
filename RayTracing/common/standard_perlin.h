// standard perlin noise.
// https://adrianb.io/2014/08/09/perlinnoise.html
// Improved perlin noise https://mrl.nyu.edu/~perlin/noise/
//

#pragma once

#include "basic.h"
#include "vec3.h"


// perlin 1d
class FPerlin1D
{
public:
	FPerlin1D();
	~FPerlin1D();

	double noise(double x) const;

protected:
	double fade(double t) const;

	static const int kSampleCount;
	double* gradient;
};


// perlin 3d
class FPerlin3D
{
public:
	FPerlin3D();
	~FPerlin3D();

	double noise(double x, double y, double z) const;
	double octaveNoise(double x, double y, double z, int octaves, double persistence) const;

protected:
	double fade(double t) const;
	int inc(int num) const;
	double grad(int hash, double x, double y, double z) const;

protected:
	int* perm;
	int repeat;
};

