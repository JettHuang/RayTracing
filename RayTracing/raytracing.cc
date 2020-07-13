// main.cc
//
//

#include <iostream>
#include "timer.h"
#include "hittable.h"
#include "hittable_aggregate.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "constantmedium.h"


static FColor3 kBlack(0, 0, 0);
static FColor3 kWhite(1, 1, 1);
static FColor3 kSkyblue(0.5, 0.7, 1.0);

FColor3 ray_color(const FRay& ray, const FColor3 &background, FHittable& world, int depth)
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


FHittableAggregate random_scene()
{
	FHittableAggregate world;

	auto checker = make_shared<FCheckerTexture>(
		make_shared<FSolidColor>(FColor3(0.2, 0.3, 0.1)),
		make_shared<FSolidColor>(FColor3(0.9, 0.9, 0.9)));
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(checker);
	world.add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));

	const int count = 11;
	for (int a = -count; a < count; a++) {
		for (int b = -count; b < count; b++) {
			auto choose_mat = random_double();
			FPoint3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - FPoint3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<FMaterial> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = FColor3::random() * FColor3::random();
					sphere_material = make_shared<FLambertian>(make_shared<FSolidColor>(albedo));

					auto center2 = center + FVec3(0, random_double(0, 0.5), 0);
					world.add(make_shared<FSphere>(center, 0.2, sphere_material));
					//world.add(make_shared<FMovingSphere>(FPositionTrackKey(center, 0.0), FPositionTrackKey(center2, 1.0), 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = FColor3::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<FMetal>(albedo, fuzz);
					world.add(make_shared<FSphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<FDielectric>(1.5);
					world.add(make_shared<FSphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<FDielectric>(1.5);
	world.add(make_shared<FSphere>(FPoint3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<FLambertian>(make_shared<FSolidColor>(FColor3(0.4, 0.2, 0.1)));
	world.add(make_shared<FSphere>(FPoint3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<FMetal>(FColor3(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<FSphere>(FPoint3(4, 1, 0), 1.0, material3));

	// use bvh
	shared_ptr<FBVH_Node> bvh = make_shared<FBVH_Node>(world, 0.0, 1.0);
	return FHittableAggregate(bvh);
}

FHittableAggregate two_spheres()
{
	FHittableAggregate world;

	auto checker = make_shared<FCheckerTexture>(
		make_shared<FSolidColor>(FColor3(0.2, 0.3, 0.1)),
		make_shared<FSolidColor>(FColor3(0.9, 0.9, 0.9)));

	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(checker);

	world.add(make_shared<FSphere>(FPoint3(0, -10, 0), 10, ground_material));
	world.add(make_shared<FSphere>(FPoint3(0, +10, 0), 10, ground_material));

	return world;
}

FHittableAggregate two_perlin_spheres()
{
	FHittableAggregate world;

	auto noise_texture = make_shared<FNoiseTexture>();

	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(noise_texture);

	world.add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));
	world.add(make_shared<FSphere>(FPoint3(0, 2, 0), 2, ground_material));

	return world;
}

FHittableAggregate scene_earth()
{
	FHittableAggregate world;

	auto earth_texture = make_shared<FImageTexture>("earthmap.jpg");

	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(earth_texture);
	world.add(make_shared<FSphere>(FPoint3(0, 0, 0), 2, ground_material));
	
	return world;
}

FHittableAggregate simple_light()
{
	FHittableAggregate world;

	auto noise_texture = make_shared<FNoiseTexture>();
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(noise_texture);

	world.add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));
	world.add(make_shared<FSphere>(FPoint3(0, 2, 0), 2, ground_material));

	auto difflight = make_shared<FDiffuseLight>(make_shared<FSolidColor>(4,4,4));
	world.add(make_shared<FSphere>(FPoint3(0, 6, 0), 2, difflight));
	world.add(make_shared<FXYRect>(3, 5, 1, 3, -2, difflight));

	return world;
}

FHittableAggregate cornell_box() {
	FHittableAggregate world;

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(15, 15, 15));

	world.add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	world.add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<FXZRect>(213, 343, 227, 332, 554, light));
	world.add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	world.add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	shared_ptr<FHittable> box1 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 330, 165), white);
	box1 = make_shared<FRotateY>(box1, 15);
	box1 = make_shared<FTranslate>(box1, FVec3(265, 0, 295));
	world.add(box1);

	shared_ptr<FHittable> box2 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 165, 165), white);
	box2 = make_shared<FRotateY>(box2, -18);
	box2 = make_shared<FTranslate>(box2, FVec3(130, 0, 65));
	world.add(box2);

	return world;
}

FHittableAggregate cornell_smoke() {
	FHittableAggregate objects;

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(7, 7, 7));

	objects.add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	objects.add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<FXZRect>(113, 443, 127, 432, 554, light));
	objects.add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	objects.add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	shared_ptr<FHittable> box1 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 330, 165), white);
	box1 = make_shared<FRotateY>(box1, 15);
	box1 = make_shared<FTranslate>(box1, FVec3(265, 0, 295));

	shared_ptr<FHittable> box2 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 165, 165), white);
	box2 = make_shared<FRotateY>(box2, -18);
	box2 = make_shared<FTranslate>(box2, FVec3(130, 0, 65));

	objects.add(make_shared<FConstantMedium>(box1, 0.01, make_shared<FSolidColor>(0, 0, 0)));
	objects.add(make_shared<FConstantMedium>(box2, 0.01, make_shared<FSolidColor>(1, 1, 1)));

	return objects;
}

FHittableAggregate final_scene() {
	FHittableAggregate objects;

	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(7, 7, 7));
	objects.add(make_shared<FXZRect>(123, 423, 147, 412, 554, light));

	FHittableAggregate boxes1;
	auto ground = make_shared<FLambertian>(make_shared<FSolidColor>(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<FBox>(FPoint3(x0, y0, z0), FPoint3(x1, y1, z1), ground));
		}
	}

	objects.add(make_shared<FBVH_Node>(boxes1, 0, 1));

	auto center1 = FPoint3(400, 400, 200);
	auto center2 = center1 + FVec3(30, 0, 0);
	auto moving_sphere_material =
		make_shared<FLambertian>(make_shared<FSolidColor>(0.7, 0.3, 0.1));
	objects.add(make_shared<FMovingSphere>(FPositionTrackKey(center1, 0), FPositionTrackKey(center2, 1), 50, moving_sphere_material));

	objects.add(make_shared<FSphere>(FPoint3(260, 150, 45), 50, make_shared<FDielectric>(1.5)));
	objects.add(make_shared<FSphere>(
		FPoint3(0, 150, 145), 50, make_shared<FMetal>(FColor3(0.8, 0.8, 0.9), 10.0)
		));

	auto boundary = make_shared<FSphere>(FPoint3(360, 150, 145), 70, make_shared<FDielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<FConstantMedium>(
		boundary, 0.2, make_shared<FSolidColor>(0.2, 0.4, 0.9)
		));
	boundary = make_shared<FSphere>(FPoint3(0, 0, 0), 5000, make_shared<FDielectric>(1.5));
	objects.add(make_shared<FConstantMedium>(boundary, .0001, make_shared<FSolidColor>(1, 1, 1)));


	auto emat = make_shared<FLambertian>(make_shared<FImageTexture>("earthmap.jpg"));
	objects.add(make_shared<FSphere>(FPoint3(400, 200, 400), 100, emat));

	auto pertext = make_shared<FNoiseTexture>(0.1);
	objects.add(make_shared<FSphere>(FPoint3(220, 280, 300), 80, make_shared<FLambertian>(pertext)));

	FHittableAggregate boxes2;
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<FSphere>(FPoint3::random(0, 165), 10, white));
	}

	objects.add(make_shared<FTranslate>(
		make_shared<FRotateY>(
			make_shared<FBVH_Node>(boxes2, 0.0, 1.0), 15),
		FVec3(-100, 270, 395)
		)
	);

	return objects;
}


int main()
{
	const FColor3 kBackground(0.0, 0.0, 0.0);

	const auto aspect_ratio = 1.0; // 16.0 / 9.0;
	const auto film_dist = 10.0;
	const int image_with = 600;
	const int image_height = static_cast<int>(image_with / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	std::cerr << "photo size: " << image_with << ", " << image_height << std::endl;
	std::cout << "P3\n" << image_with << " " << image_height << "\n255\n";

	// world
	FHittableAggregate world = final_scene(); // cornell_box();//random_scene();

#if 0
	FPoint3 lookfrom(13, 2, 3);
	FPoint3 lookat(0, 0, 0);
	FVec3 vup(0, 1, 0);
	double vfov = 40.0;
#else
	FPoint3 lookfrom(478, 278, -600);
	FPoint3 lookat(278, 278, 0);
	FVec3 vup(0, 1, 0);
	double vfov = 40.0;
#endif

#if 1 // use pin-hole camera
	FCameraWithShuter camera(lookfrom, lookat, vup, vfov, aspect_ratio, film_dist, 0.0, 1.0);

#else // use thin-lens camera
	auto film_dist = 2.0;
	auto aperture = 0.2;
	auto focus_length = 1.5;

	FCameraWithLensShutter camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_length, film_dist, image_height, 1, 0.0, 1.0);
	double depthFieldFar, depthFieldNear;
	camera.GetDepthOfField(depthFieldFar, depthFieldNear);
	std::cerr << "depth of field: far=" << depthFieldFar << ", near=" << depthFieldNear << std::endl;
#endif

	FPerformanceCounter PerfCounter;
	PerfCounter.StartPerf();
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

				FRay ray = camera.castRay(u, v);
				pixel_color += ray_color(ray, kBackground, world, max_depth);
			}

			write_color(std::cout, pixel_color, samples_per_pixel, 2.2);
		}
	} // end j

	double elapse_ms = PerfCounter.EndPerf();
	std::cerr << "performance seconds: " << std::fixed << (elapse_ms / 1000000.0) << std::endl;
	std::cerr << "\nDone.\n";
	return 0;
}
