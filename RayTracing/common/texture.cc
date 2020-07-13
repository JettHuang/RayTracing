// texture
//
//

#include "texture.h"
#include "rtw_stb_image.h"


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
