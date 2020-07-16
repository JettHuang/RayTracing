// texture
//

#pragma once

#include <algorithm>
#include "basic.h"
#include "vec3.h"
#include "value_noise.h"
#include "classic_noise.h"
#include "simplex_noise.h"
#include "worley_noise.h"


// class texture
class FTexture
{
public:
	virtual FColor3 value(double u, double v, const FPoint3 &p) const = 0;
};


// solid color
class FSolidColor : public FTexture
{
public:
	FSolidColor() {}
	FSolidColor(const FColor3& c) : color(c) {}
	FSolidColor(double r, double g, double b)
		: color(r,g,b)
	{}

	virtual FColor3 value(double u, double v, const FPoint3& p) const override
	{
		return color;
	}

private:
	FColor3	color;
};

// checker texture
class FCheckerTexture : public FTexture
{
public:
	FCheckerTexture() {}
	FCheckerTexture(const shared_ptr<FTexture>& t0, const shared_ptr<FTexture>& t1) : odd(t0), even(t1) {}

	virtual FColor3 value(double u, double v, const FPoint3& p) const override;

public:
	shared_ptr<FTexture> odd;
	shared_ptr<FTexture> even;
};

// noise texture
#define NOISE_EFFECT_SUNSURFACE  0
#define NOISE_EFFECT_MARBLE		 1
#define NOISE_EFFECT_WOOD		 2
#define NOISE_EFFECT_WORLEY		 3

class FNoiseTexture : public FTexture
{
public:
	FNoiseTexture() : effectType(NOISE_EFFECT_WOOD) {}
	FNoiseTexture(int effect) : effectType(effect) {}

	virtual FColor3 value(double u, double v, const FPoint3& p) const override;

protected:
	FColor3 sun_surface_effect(const FPoint3& p) const;
	FColor3 marble_effect(const FPoint3& p) const;
	FColor3 wood_effect(const FPoint3& p) const;
	FColor3 worley_effect(const FPoint3 &p) const;

protected:
	int			 effectType;
	ValueNoise   valueNoise;
	ClassicNoise classic;
	SimplexNoise simplex;
	WorleyNoise  worley;
};

// image texture
class FImageTexture : public FTexture
{
public:
	const static int bytes_per_pixel = 3;

	FImageTexture()
		: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	FImageTexture(const char* filename);

	~FImageTexture() {
		delete data;
	}

	virtual FColor3 value(double u, double v, const FPoint3& p) const override;

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};
