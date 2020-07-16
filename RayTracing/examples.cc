// example.cc
//
//

#include <iostream>
#include "timer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "constantmedium.h"


shared_ptr<FHittable> sample_random_scene(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(13, 2, 3);
	const FPoint3 lookat(0,0,0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0.70, 0.80, 1.00);

	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto checker = make_shared<FCheckerTexture>(
		make_shared<FSolidColor>(FColor3(0.2, 0.3, 0.1)),
		make_shared<FSolidColor>(FColor3(0.9, 0.9, 0.9)));
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(checker);
	world->add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));

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
					world->add(make_shared<FMovingSphere>(FPositionTrackKey(center, 0.0), FPositionTrackKey(center2, 1.0), 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = FColor3::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<FMetal>(albedo, fuzz);
					world->add(make_shared<FSphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<FDielectric>(1.5);
					world->add(make_shared<FSphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<FDielectric>(1.5);
	world->add(make_shared<FSphere>(FPoint3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<FLambertian>(make_shared<FSolidColor>(FColor3(0.4, 0.2, 0.1)));
	world->add(make_shared<FSphere>(FPoint3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<FMetal>(FColor3(0.7, 0.6, 0.5), 0.0);
	world->add(make_shared<FSphere>(FPoint3(4, 1, 0), 1.0, material3));

	// use bvh
	shared_ptr<FBVH_Node> bvh = make_shared<FBVH_Node>(*world, 0.0, 1.0);
	return bvh;
}

shared_ptr<FHittable> sample_two_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(13, 2, 3);
	const FPoint3 lookat(0, 0, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0.70, 0.80, 1.00);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto checker = make_shared<FCheckerTexture>(
		make_shared<FSolidColor>(FColor3(0.2, 0.3, 0.1)),
		make_shared<FSolidColor>(FColor3(0.9, 0.9, 0.9)));

	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(checker);

	world->add(make_shared<FSphere>(FPoint3(0, -10, 0), 10, ground_material));
	world->add(make_shared<FSphere>(FPoint3(0, +10, 0), 10, ground_material));

	return world;
}

shared_ptr<FHittable> sample_two_perlin_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(13, 2, 3);
	const FPoint3 lookat(0, 0, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0.70, 0.80, 1.00);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto noise_texture1 = make_shared<FNoiseTexture>(NOISE_EFFECT_WOOD);
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(noise_texture1);

	auto noise_texture2 = make_shared<FNoiseTexture>(NOISE_EFFECT_MARBLE);
	shared_ptr<FMaterial> ground_material2 = make_shared<FLambertian>(noise_texture2);

	world->add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material2));
	world->add(make_shared<FSphere>(FPoint3(0, 2, 0), 2, ground_material));

	return world;
}

shared_ptr<FHittable> sample_two_worley_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(13, 6, 40);
	const FPoint3 lookat(0, 0, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0.70, 0.80, 1.00);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto noise_texture = make_shared<FNoiseTexture>(NOISE_EFFECT_WORLEY);
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(noise_texture);

	world->add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));
	world->add(make_shared<FSphere>(FPoint3(0, 4, 0), 4, ground_material));

	return world;
}

shared_ptr<FHittable> sample_earth(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(0, 0, 12);
	const FPoint3 lookat(0, 0, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0.70, 0.80, 1.00);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto earth_texture = make_shared<FImageTexture>("earthmap.jpg");
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(earth_texture);
	world->add(make_shared<FSphere>(FPoint3(0, 0, 0), 2, ground_material));

	return world;
}

shared_ptr<FHittable> sample_simple_light(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(26, 3, 6);
	const FPoint3 lookat(0, 2, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 20.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto noise_texture = make_shared<FNoiseTexture>();
	shared_ptr<FMaterial> ground_material = make_shared<FLambertian>(noise_texture);

	world->add(make_shared<FSphere>(FPoint3(0, -1000, 0), 1000, ground_material));
	world->add(make_shared<FSphere>(FPoint3(0, 2, 0), 2, ground_material));

	auto difflight = make_shared<FDiffuseLight>(make_shared<FSolidColor>(4, 4, 4));
	world->add(make_shared<FSphere>(FPoint3(0, 7, 0), 2, difflight));
	world->add(make_shared<FXYRect>(3, 5, 1, 3, -2, difflight));

	return world;
}

shared_ptr<FHittable> sample_cornell_box(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(278, 278, -800);
	const FPoint3 lookat(278, 278, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 40.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(15, 15, 15));

	world->add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	world->add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	world->add(make_shared<FXZRect>(213, 343, 227, 332, 554, light));
	world->add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	world->add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	world->add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	shared_ptr<FHittable> box1 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 330, 165), white);
	box1 = make_shared<FRotateY>(box1, 15);
	box1 = make_shared<FTranslate>(box1, FVec3(265, 0, 295));
	world->add(box1);

	shared_ptr<FHittable> box2 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 165, 165), white);
	box2 = make_shared<FRotateY>(box2, -18);
	box2 = make_shared<FTranslate>(box2, FVec3(130, 0, 65));
	world->add(box2);

	return world;
}

shared_ptr<FHittable> sample_cornell_ball(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(278, 278, -800);
	const FPoint3 lookat(278, 278, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 40.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(5, 5, 5));

	world->add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	world->add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	world->add(make_shared<FXZRect>(113, 443, 127, 432, 554, light));
	world->add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	world->add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	world->add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	auto boundary = make_shared<FSphere>(FPoint3(160, 100, 145), 100, make_shared<FDielectric>(1.5));
	world->add(boundary);
	world->add(make_shared<FConstantMedium>(boundary, 0.1, make_shared<FSolidColor>(1, 1, 1)));

	shared_ptr<FHittable> box1 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 330, 165), white);
	box1 = make_shared<FRotateY>(box1, 15);
	box1 = make_shared<FTranslate>(box1, FVec3(265, 0, 295));
	world->add(box1);

	return world;
}

shared_ptr<FHittable> sample_cornell_smoke(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(278, 278, -800);
	const FPoint3 lookat(278, 278, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 40.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);

	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(7, 7, 7));

	world->add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	world->add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	world->add(make_shared<FXZRect>(113, 443, 127, 432, 554, light));
	world->add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	world->add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	world->add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	shared_ptr<FHittable> box1 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 330, 165), white);
	box1 = make_shared<FRotateY>(box1, 15);
	box1 = make_shared<FTranslate>(box1, FVec3(265, 0, 295));

	shared_ptr<FHittable> box2 = make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 165, 165), white);
	box2 = make_shared<FRotateY>(box2, -18);
	box2 = make_shared<FTranslate>(box2, FVec3(130, 0, 65));

	world->add(make_shared<FConstantMedium>(box1, 0.01, make_shared<FSolidColor>(0, 0, 0)));
	world->add(make_shared<FConstantMedium>(box2, 0.01, make_shared<FSolidColor>(1, 1, 1)));

	return world;
}

shared_ptr<FHittable> sample_cornell_final(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(278, 278, -800);
	const FPoint3 lookat(278, 278, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 40.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto pertext = make_shared<FNoiseTexture>();

	auto mat = make_shared<FLambertian>(make_shared<FImageTexture>("earthmap.jpg"));

	auto red = make_shared<FLambertian>(make_shared<FSolidColor>(.65, .05, .05));
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	auto green = make_shared<FLambertian>(make_shared<FSolidColor>(.12, .45, .15));
	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(7, 7, 7));

	world->add(make_shared<FFlipFace>(make_shared<FYZRect>(0, 555, 0, 555, 555, green)));
	world->add(make_shared<FYZRect>(0, 555, 0, 555, 0, red));
	world->add(make_shared<FXZRect>(123, 423, 147, 412, 554, light));
	world->add(make_shared<FFlipFace>(make_shared<FXZRect>(0, 555, 0, 555, 555, white)));
	world->add(make_shared<FXZRect>(0, 555, 0, 555, 0, white));
	world->add(make_shared<FFlipFace>(make_shared<FXYRect>(0, 555, 0, 555, 555, white)));

	shared_ptr<FHittable> boundary2 =
		make_shared<FBox>(FPoint3(0, 0, 0), FPoint3(165, 165, 165), make_shared<FDielectric>(1.5));
	boundary2 = make_shared<FRotateY>(boundary2, -18);
	boundary2 = make_shared<FTranslate>(boundary2, FVec3(130, 0, 65));

	auto tex = make_shared<FSolidColor>(0.9, 0.9, 0.9);

	world->add(boundary2);
	world->add(make_shared<FConstantMedium>(boundary2, 0.2, tex));

	return world;
}

shared_ptr<FHittable> sample_final_scene(shared_ptr<FRayCamera>& OutCamera, FColor3& background)
{
	const auto aspect_ratio = 1.0 / 1.0;
	const FPoint3 lookfrom(278, 278, -600);
	const FPoint3 lookat(278, 278, 0);
	const FVec3 vup(0, 1, 0);
	auto vfov = 40.0;
	auto aperture = 0.0;
	auto film_focus = 10.0;
	OutCamera = make_shared<FPinholeCamera>(lookfrom, lookat, vup, vfov, aspect_ratio, film_focus, 0.0, 0.0);
	background = FColor3(0, 0, 0);


	shared_ptr<FHittableList> world = make_shared<FHittableList>();

	auto light = make_shared<FDiffuseLight>(make_shared<FSolidColor>(7, 7, 7));
	world->add(make_shared<FXZRect>(123, 423, 147, 412, 554, light));

	FHittableList boxes1;
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

	world->add(make_shared<FBVH_Node>(boxes1, 0, 1));

	auto center1 = FPoint3(400, 400, 200);
	auto center2 = center1 + FVec3(30, 0, 0);
	auto moving_sphere_material =
		make_shared<FLambertian>(make_shared<FSolidColor>(0.7, 0.3, 0.1));
	world->add(make_shared<FMovingSphere>(FPositionTrackKey(center1, 0), FPositionTrackKey(center2, 1), 50, moving_sphere_material));

	world->add(make_shared<FSphere>(FPoint3(260, 150, 45), 50, make_shared<FDielectric>(1.5)));
	world->add(make_shared<FSphere>(
		FPoint3(0, 150, 145), 50, make_shared<FMetal>(FColor3(0.8, 0.8, 0.9), 10.0)
		));

	auto boundary = make_shared<FSphere>(FPoint3(360, 150, 145), 70, make_shared<FDielectric>(1.5));
	world->add(boundary);
	world->add(make_shared<FConstantMedium>(
		boundary, 0.2, make_shared<FSolidColor>(0.2, 0.4, 0.9)
		));
	boundary = make_shared<FSphere>(FPoint3(0, 0, 0), 5000, make_shared<FDielectric>(1.5));
	world->add(make_shared<FConstantMedium>(boundary, .0001, make_shared<FSolidColor>(1, 1, 1)));


	auto emat = make_shared<FLambertian>(make_shared<FImageTexture>("earthmap.jpg"));
	world->add(make_shared<FSphere>(FPoint3(400, 200, 400), 100, emat));

	auto pertext = make_shared<FNoiseTexture>();
	world->add(make_shared<FSphere>(FPoint3(220, 280, 300), 80, make_shared<FLambertian>(pertext)));

	FHittableList boxes2;
	auto white = make_shared<FLambertian>(make_shared<FSolidColor>(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<FSphere>(FPoint3::random(0, 165), 10, white));
	}

	world->add(make_shared<FTranslate>(
		make_shared<FRotateY>(
			make_shared<FBVH_Node>(boxes2, 0.0, 1.0), 15),
		FVec3(-100, 270, 395)
		)
	);

	return world;
}


