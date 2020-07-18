// example.cc
//
//

#include <iostream>
#include "basic.h"
#include "timer.h"
#include "color.h"
#include "hittable.h"
#include "material.h"
#include "examples.h"


// all examples
struct FExampleDesc{
	const char* _name;
	ExampleFuncPtr _funcptr;
};

FExampleDesc examples[] = {
	{ "random scen", sample_random_scene},
	{ "two spheres", sample_two_spheres},
	{ "perlin spheres", sample_two_perlin_spheres},
	{ "worley spheres", sample_two_worley_spheres},
	{ "earth", sample_earth },
	{ "simple light", sample_simple_light},
	{ "cornell box", sample_cornell_box },
	{ "cornell ball", sample_cornell_ball },
	{ "cornell smoke", sample_cornell_smoke },
	{ "cornell final", sample_cornell_final },
	{ "final scene", sample_final_scene }
};

static FColor3 kBlack(0, 0, 0);
static FColor3 kWhite(1, 1, 1);
static FColor3 kSkyblue(0.5, 0.7, 1.0);

FColor3 ray_color(const FRay& ray, const FColor3& background, FHittable& world, int depth)
{
	FHitRecord rec;

	// if we have exceeded the ray bounce limit, no more light is gathered
	if (depth <= 0)
	{
		return kBlack;
	}

	if (!world.hit(ray, 0.001, kInfinity, rec)) {
		return background;
	}

	FRay scattered;
	FColor3 attenuation;
	FColor3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
	{
		return emitted;
	}

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

// monte-carlo path trace
// P_RR: Russian Roulette property
FColor3 ray_color_montecarlo(const FRay& ray, const FColor3& background, FHittable& world, const double& P_RR)
{
	FHitRecord rec;



	if (!world.hit(ray, 0.001, kInfinity, rec)) {
		return background;
	}

	FRay scattered;
	FColor3 attenuation;
	FColor3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
	{
		return emitted;
	}

	double ksi = random_double();
	if (ksi <= P_RR)
	{
		FColor3 L_indir = (attenuation* ray_color_montecarlo(scattered, background, world, P_RR) / rec.mat_ptr->pdf(scattered.Direction())) / P_RR;
		return emitted + L_indir;
	}

	return emitted;
}


void display_usage()
{
	std::cerr << "Usage:  program.exe sceneId  methodId > filename.ppm" << std::endl;
	for (int i=0; i< sizeof(examples) / sizeof(examples[0]); ++i)
	{
		std::cerr << "   " << i << ". " << examples[i]._name << std::endl;
	}
}

int main(int argc, char* argv[])
{
	FColor3 kBackground(0.0, 0.0, 0.0);

	const auto aspect_ratio = 1.0;
	const auto film_dist = 10.0;
	const int image_with = 600;
	const int image_height = static_cast<int>(image_with / aspect_ratio);

	int trace_method = 0; // 0: normal, 1: monte carlo
	int example_index = -1;
	if (argc > 1)
	{
		example_index = atoi(argv[1]);
		if (argc > 2) {
			trace_method = 1;
		}
	}
	if (example_index < 0 || example_index >= sizeof(examples) / sizeof(examples[0]))
	{
		display_usage();
		return 0;
	}

	shared_ptr<FRayCamera> camera = nullptr;
	shared_ptr<FHittable> theWorld = examples[example_index]._funcptr(camera, kBackground);

	std::cerr << "photo size: " << image_with << ", " << image_height << std::endl;
	std::cout << "P3\n" << image_with << " " << image_height << "\n255\n";
	FPerformanceCounter PerfCounter;
	PerfCounter.StartPerf();

	if (trace_method == 0)
	{
		const int samples_per_pixel = 100;
		const int max_depth = 50;

		for (int j = image_height - 1; j >= 0; --j)
		{
			std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < image_with; ++i)
			{
				FColor3 pixel_color(0, 0, 0);

				for (int s = 0; s < samples_per_pixel; ++s)
				{
					auto u = (i + random_double()) / (image_with - 1);
					auto v = (j + random_double()) / (image_height - 1);

					FRay ray = camera->castRay(u, v);
					pixel_color += ray_color(ray, kBackground, *theWorld, max_depth);
				}

				write_color(std::cout, pixel_color, samples_per_pixel, 2.2);
			}
		} // end j
	}
	else if (trace_method == 1)
	{
		const int samples_per_pixel = 10000;
		const double P_RR = 0.75;

		for (int j = image_height - 1; j >= 0; --j)
		{
			std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < image_with; ++i)
			{
				FColor3 pixel_color(0, 0, 0);

				for (int s = 0; s < samples_per_pixel; ++s)
				{
					auto u = (i + random_double()) / (image_with - 1);
					auto v = (j + random_double()) / (image_height - 1);

					FRay ray = camera->castRay(u, v);
					pixel_color += ray_color_montecarlo(ray, kBackground, *theWorld, 0.6);
				}

				write_color(std::cout, pixel_color, samples_per_pixel, 2.2);
			}
		} // end j
	}

	double elapse_ms = PerfCounter.EndPerf();
	std::cerr << "performance seconds: " << std::fixed << (elapse_ms / 1000000.0) << std::endl;
	std::cerr << "\nDone.\n";
	return 0;
}