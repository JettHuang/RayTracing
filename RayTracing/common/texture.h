// texture
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "perlin.h"
#include "standard_perlin.h"
#include "SimplexNoise.h"


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
class FNoiseTexture : public FTexture
{
public:
	FNoiseTexture() : scale(1.0) {}
	FNoiseTexture(double sc) : scale(sc) {}

	virtual FColor3 value(double u, double v, const FPoint3& p) const override
	{
		//return FColor3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
		return FColor3(1, 1, 1) * ((simplex.fractal(7, p.x(), p.y(), p.z()) + 1.0) * 0.5);
	}

public:
	FPerlin	noise;
	FPerlin1D noise1d;
	SimplexNoise simplex;
	double	scale;
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
