/* worley noise for 1d, 2d, 3d */

#include "worley_noise.h"
#include "basic.h"
#include "vec3.h"


static const uint32_t OFFSET_BASIS = 2166136261;
static const uint32_t FNV_PRIME = 16777619;
static const uint32_t LCG_RANDOM_MAX = 0xFFFFFFFFu;
static const uint32_t LCG_SEED = 1376312589;

static uint32_t hash(uint32_t i)
{
	return (uint32_t)((OFFSET_BASIS ^ i) * FNV_PRIME);
}

static uint32_t hash(uint32_t i, uint32_t j)
{
	return (uint32_t)((((OFFSET_BASIS ^ i) * FNV_PRIME) ^ j) * FNV_PRIME);
}

static uint32_t hash(uint32_t i, uint32_t j, uint32_t k)
{
	return (uint32_t)((((((OFFSET_BASIS ^ i) * FNV_PRIME) ^ j) * FNV_PRIME) ^ k) * FNV_PRIME);
}

static uint32_t lcg_random(uint32_t lastValue)
{
	return (uint32_t)((1103515245u * lastValue + 12345u) % LCG_RANDOM_MAX);
}

static int32_t getFeaturePointsCount(uint32_t h)
{
	if (h < 393325350)  return 1;
	if (h < 2700834071) return 2;

	return 3;
}

// euclidian 
static inline float distance(float x0, float x1)
{
	return fabs(x0 - x1);
}

static inline float distance(float x0, float y0, float x1, float y1)
{
	float x = x0 - x1;
	float y = y0 - y1;
	return sqrtf(x*x + y*y);
}

static inline float distance(float x0, float y0, float z0, float x1, float y1, float z1)
{
	float x = x0 - x1;
	float y = y0 - y1;
	float z = z0 - z1;
	return sqrtf(x * x + y * y + z * z);
}

float WorleyNoise::noise(float x)
{
	uint32_t lastRandom;
	float mindist = 10.f;

	int32_t cur_xi = (int32_t)floor(x);
	for (int32_t i = -1; i < 2; ++i) // -1 0 +1
	{
		int32_t xi = cur_xi + i;
		lastRandom = lcg_random(hash((uint32_t)xi + LCG_SEED));
		int32_t points_count = getFeaturePointsCount(lastRandom);
		for (int32_t m=0; m<points_count; ++m)
		{
			lastRandom = lcg_random(lastRandom);
			float px = (float)lastRandom / (float)LCG_RANDOM_MAX;
			float dist = distance(x, xi + px);
			mindist = std::min(mindist, dist);
		} // end for m
	} // end for i

	return mindist;
}

float WorleyNoise::noise(float x, float y)
{
	uint32_t lastRandom;
	float mindist = 10.f;

	int32_t cur_xi = (int32_t)floor(x);
	int32_t cur_yi = (int32_t)floor(y);
	for (int32_t i = -1; i < 2; ++i) // -1 0 +1
	{
		for (int32_t j = -1; j < 2; ++j)
		{
			int32_t xi = cur_xi + i;
			int32_t yi = cur_yi + j;

			lastRandom = lcg_random(hash((uint32_t)xi + LCG_SEED, (uint32_t)yi));
			int32_t points_count = getFeaturePointsCount(lastRandom);
			for (int32_t m = 0; m < points_count; ++m)
			{
				lastRandom = lcg_random(lastRandom);
				float px = (float)lastRandom / (float)LCG_RANDOM_MAX;
				
				lastRandom = lcg_random(lastRandom);
				float py = (float)lastRandom / (float)LCG_RANDOM_MAX;

				float dist = distance(x, y, xi+px, yi+py);
				mindist = std::min(mindist, dist);
			} // end for m
		} // for j

	} // end for i

	return mindist;
}

float WorleyNoise::noise(float x, float y, float z)
{
	uint32_t lastRandom;
	float mindist = 10.f;

	int32_t cur_xi = (int32_t)floor(x);
	int32_t cur_yi = (int32_t)floor(y);
	int32_t cur_zi = (int32_t)floor(z);
	for (int32_t i = -1; i < 2; ++i) // -1 0 +1
	{
		for (int32_t j = -1; j < 2; ++j)
		{
			for (int32_t k = -1; k < 2; ++k)
			{
				int32_t xi = cur_xi + i;
				int32_t yi = cur_yi + j;
				int32_t zi = cur_zi + k;

				lastRandom = lcg_random(hash((uint32_t)xi + LCG_SEED, (uint32_t)yi, (uint32_t)zi));
				int32_t points_count = getFeaturePointsCount(lastRandom);
				for (int32_t m = 0; m < points_count; ++m)
				{
					lastRandom = lcg_random(lastRandom);
					float px = (float)lastRandom / (float)LCG_RANDOM_MAX;

					lastRandom = lcg_random(lastRandom);
					float py = (float)lastRandom / (float)LCG_RANDOM_MAX;

					lastRandom = lcg_random(lastRandom);
					float pz = (float)lastRandom / (float)LCG_RANDOM_MAX;

					float dist = distance(x, y, z, xi + px, yi + py, zi + pz);
					mindist = std::min(mindist, dist);
				} // end for k
			}
			
		}

	} // end for i

	return mindist;
}

float WorleyNoise::fractal(size_t octaves, float x) const
{
	float output = 0.f;
	float denom = 0.f;
	float frequency = mFrequency;
	float amplitude = mAmplitude;

	for (size_t i = 0; i < octaves; i++) {
		output += (amplitude * noise(x * frequency));
		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	return (output / denom);
}

float WorleyNoise::fractal(size_t octaves, float x, float y) const
{
	float output = 0.f;
	float denom = 0.f;
	float frequency = mFrequency;
	float amplitude = mAmplitude;

	for (size_t i = 0; i < octaves; i++) {
		output += (amplitude * noise(x * frequency, y * frequency));
		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	return (output / denom);
}

float WorleyNoise::fractal(size_t octaves, float x, float y, float z) const
{
	float output = 0.f;
	float denom = 0.f;
	float frequency = mFrequency;
	float amplitude = mAmplitude;

	for (size_t i = 0; i < octaves; i++) {
		output += (amplitude * noise(x * frequency, y * frequency, z * frequency));
		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	return (output / denom);
}
