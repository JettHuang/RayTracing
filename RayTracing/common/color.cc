// Color3 Implementation
//
//

#include "color.h"


void write_color(std::ostream& out, FColor3& pixel_color, int samples_per_pixel, double gamma)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	// Divide the color by the number of samples and gamma-correct for gamma=2.0.
	auto oneOverGamma = 1.0 / gamma;
	auto scale = 1.0 / samples_per_pixel;
	
	r = pow(scale * r, oneOverGamma);
	g = pow(scale * g, oneOverGamma);
	b = pow(scale * b, oneOverGamma);

	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
