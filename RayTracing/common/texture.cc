// texture
//
//

#include "texture.h"
#include "rtw_stb_image.h"

// checker
FColor3 FCheckerTexture::value(double u, double v, const FPoint3& p) const
{
	auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
	if (sines < 0.0) {
		return odd->value(u, v, p);
	}
	else {
		return even->value(u, v, p);
	}
}

// noise texture
FColor3 FNoiseTexture::value(double u, double v, const FPoint3& p) const
{
	switch (effectType)
	{
	case NOISE_EFFECT_SUNSURFACE:
		return sun_surface_effect(p);
	case NOISE_EFFECT_MARBLE:
		return marble_effect(p);
	case NOISE_EFFECT_WOOD:
	default:
		return wood_effect(p);
	}

	return wood_effect(p);
}

FColor3 FNoiseTexture::sun_surface_effect(const FPoint3& p) const
{
	static const FColor3 color1(0.8, 0.7, 0.0);
	static const FColor3 color2(0.6, 0.1, 0.0);

	double noiseval = (simplex.fractal(5, (float)p.x(), (float)p.y(), (float)p.z()) + 1.0) * 0.5;
	double intensity = abs(noiseval - 0.25) +
		abs(noiseval - 0.125) +
		abs(noiseval - 0.0625) +
		abs(noiseval - 0.03125);
	intensity = clamp(intensity, 0.0, 1.0);
	FColor3 color = lerp(color1, color2, intensity);
	return color;
}

FColor3 FNoiseTexture::marble_effect(const FPoint3& p) const
{
	static const FColor3 color1(0.6, 0.77, 0.08);
	static const FColor3 color2(0.1, 0.1, 0.0);

	float noiseval = 0.f;
	float frequency = 1.0f;
	float amplitude = 1.0f;
	float lacunarity = 2.0f;
	float persistence = 0.5f;
	size_t octaves = 5;

	for (size_t i = 0; i < octaves; i++) {
		noiseval += (amplitude * simplex.noise((float)p.x() * frequency, (float)p.y() * frequency, (float)p.z() * frequency));

		frequency *= lacunarity;
		amplitude *= persistence;
	}

	double sineval = (sin(((float)p.x() + noiseval * 100.f) * 2 * kPi / 200.f) + 1) * 0.5f;
	FColor3 color = lerp(color1, color2, sineval);
	return color;
}

FColor3 FNoiseTexture::wood_effect(const FPoint3& p) const
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

	FVec3 color = lerp(LightWood, DarkWood, r);
	r = fract((p.x() + p.z()) * GrainScale + 0.5);
	noiseVec[2] *= r;
	if (r < GrainThreshold)
		color += LightWood * LightGrains * noiseVec[2];
	else
		color -= LightWood * DarkGrains * noiseVec[2];

	return color;
}


// image texture
FImageTexture::FImageTexture(const char* filename)
{
	auto components_per_pixel = bytes_per_pixel;

	data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

	if (!data) {
		std::cerr << "ERROR: Could not load texture image file" << filename << ".\n";
		width = height = 0;
	}

	bytes_per_scanline = bytes_per_pixel * width;
}

FColor3 FImageTexture::value(double u, double v, const FPoint3& p) const
{
	// if we have no texture data, then return solid cyan as a debugging aid
	if (data == nullptr) {
		return FColor3(0, 1, 1);
	}

	// Clamp input texture coordinates to [0,1]x[1,0]
	u = clamp(u, 0.0, 1.0);
	v = 1.0 - clamp(v, 0.0, 1.0); // flip v to image coordinates

	auto i = static_cast<int>(u * width);
	auto j = static_cast<int>(v * height);

	// clamp integer mapping, sine actual coordinates should be less than 1.0
	if (i >= width) i = width - 1;
	if (j >= height) j = height - 1;

	const auto color_scale = 1.0 / 255.0;
	auto pixel = data + (j * bytes_per_scanline + i * bytes_per_pixel);

	return FColor3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}
