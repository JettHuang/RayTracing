// ray tracing samples
//
//
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "hittable.h"
#include "camera.h"

typedef shared_ptr<FHittable>(*ExampleFuncPtr)(shared_ptr<FRayCamera>& OutCamera, FColor3& background);

shared_ptr<FHittable> sample_random_scene(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_two_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_two_perlin_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_two_worley_spheres(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_earth(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_simple_light(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_cornell_box(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_cornell_ball(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_cornell_smoke(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_cornell_final(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
shared_ptr<FHittable> sample_final_scene(shared_ptr<FRayCamera>& OutCamera, FColor3& background);
