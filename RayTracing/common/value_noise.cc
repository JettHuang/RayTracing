/* coherent noise function over 1, 2 or 3 dimensions */

#include "value_noise.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g1[B + B + 2];
static bool start = 1;

static void init(void);

static inline int fastfloor(float fp) {
	int i = static_cast<int>(fp);
	return (fp < i) ? (i - 1) : (i);
}

#define random()  rand()

#define s_curve(t) ( t * t * (3.f - 2.f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#if 1
#define setup(i,b0,b1,r0)\
	t = vec[i] + N; /*convert to positive number*/ \
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;
#else
#define setup(i,b0,b1,r0)\
	t = vec[i]; \
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - fastfloor(t);
#endif

static float noise1(float arg)
{
	int bx0, bx1;
	float rx0, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0, bx1, rx0);

	sx = s_curve(rx0);

	u = g1[p[bx0]];
	v = g1[p[bx1]];

	return lerp(sx, u, v);
}

static float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, ry0, sx, sy, a, b, t, u, v;
	int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0, bx1, rx0);
	setup(1, by0, by1, ry0);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

	u = g1[b00];
	v = g1[b10];
	a = lerp(sx, u, v);

	u = g1[b01];
	v = g1[b11];
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

static float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, ry0, rz0, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0, bx1, rx0);
	setup(1, by0, by1, ry0);
	setup(2, bz0, bz1, rz0);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	u = g1[b00 + bz0];
	v = g1[b10 + bz0];
	a = lerp(t, u, v);

	u = g1[b01 + bz0];
	v = g1[b11 + bz0];
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	u = g1[b00 + bz1];
	v = g1[b10 + bz1];
	a = lerp(t, u, v);

	u = g1[b01 + bz1];
	v = g1[b11 + bz1];
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

static void init(void)
{
	int i, j, k;

	for (i = 0; i < B; i++) {
		p[i] = i; // initialize permutation

		g1[i] = (float)((random() % (B + B)) - B) / B; // one-dimension gradient [-1.0, +1.0] 
	}

	// permutation(disarrange)
	while (--i) { 
		k = p[i];
		p[i] = p[j = random() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
	}
}

//////////////////////////////////////////////////////////////////////////

float ValueNoise::noise(float x)
{
	return noise1(x);
}

float ValueNoise::noise(float x, float y)
{
	float v[2] = { x, y };
	return noise2(v);
}

float ValueNoise::noise(float x, float y, float z)
{
	float v[3] = { x, y, z };
	return noise3(v);
}

float ValueNoise::fractal(size_t octaves, float x) const
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

float ValueNoise::fractal(size_t octaves, float x, float y) const
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

float ValueNoise::fractal(size_t octaves, float x, float y, float z) const
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
