// texture
//

#pragma once

#include <algorithm>
#include "basic.h"
#include "vec3.h"
#include "classic_noise.h"
#include "simplex_noise.h"


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
		//double value = (simplex.fractal(2, p.x(), p.y(), p.z()) + 1.0) * 0.5;
		//return FColor3(1,1,1) * 0.5 * (1 + sin(scale * p.z() + 10 * value));
		return wood_effect(p);
	}

protected:
	FColor3 sun_surface_effect(const FPoint3& p) const
	{
		static const FColor3 color1(0.8,0.7,0.0);
		static const FColor3 color2(0.6,0.1,0.0);

		double noiseval = (simplex.fractal(5, (float)p.x(), (float)p.y(), (float)p.z()) + 1.0) * 0.5;
		double intensity = abs(noiseval - 0.25) + 
						   abs(noiseval - 0.125) + 
			               abs(noiseval - 0.0625) + 
			               abs(noiseval - 0.03125);
		intensity = clamp(intensity, 0.0, 1.0);
		FColor3 color = slerp(color1, color2, intensity);
		return color;
	}

	FColor3 marble_effect(const FPoint3& p) const
	{
		static const FColor3 color1(0.6, 0.77, 0.08);
		static const FColor3 color2(0.1, 0.1, 0.0);

		double noiseval = (simplex.fractal(5, (float)p.x(), (float)p.y(), (float)p.z()) + 1.0) * 0.5;
		double intensity = abs(noiseval - 0.25) +
						   abs(noiseval - 0.125) +
			               abs(noiseval - 0.0625) +
			               abs(noiseval - 0.03125);
		double sineval = sin(p.y() * 16.0 + intensity * 12.0) * 0.5 + 0.5;
		FColor3 color = slerp(color1, color2, sineval);
		return color;
	}

	FColor3 wood_effect(const FPoint3& p) const
	{
		static const FVec3 LightWood(0.6, 0.3, 0.1);
		static const FVec3 DarkWood(0.4, 0.2, 0.07);
		static const float RingFreq = 4.0;
		static const float LightGrains = 1.0;
		static const float DarkGrains = 0.0;
		static const float GrainThreshold = 0.5;
		static const FVec3 NoiseScale(0.5, 0.1, 0.1);
		static const float Noisiness = 3.0;
		static const float GrainScale = 27.0;

		FPoint3 sp = NoiseScale * p;
		double noiseval = (simplex.fractal(5, (float)sp.x(), (float)sp.y(), (float)sp.z()) + 1.0) * 0.5;
		FVec3 noiseVec = Noisiness * FVec3(noiseval, noiseval, noiseval);
		FVec3 location = p + noiseVec;
		double dist = sqrt(location.x() * location.x() + location.z() * location.z());
		dist *= RingFreq;

		double r = fract(dist + noiseVec[0] + noiseVec[1] + noiseVec[2]) * 2.0;
		if (r > 1.0) { r = 2.0 - r; }

		FVec3 color = slerp(LightWood, DarkWood, r);
		r = fract((p.x() + p.z()) * GrainScale + 0.5);
		noiseVec[2] *= r;
		if (r < GrainThreshold)
			color += LightWood * LightGrains * noiseVec[2];
		else
			color -= LightWood * DarkGrains * noiseVec[2];

		return color;
	}

protected:
	ClassicNoise classic;
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
