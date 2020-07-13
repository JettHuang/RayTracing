// hittables aggregate
//
//

#include "hittable_aggregate.h"

bool FHittableAggregate::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	FHitRecord temp_hit;
	auto hit_any = false;
	auto closest_sofar = t_max;

	for (const auto &object : objects)
	{
		if (object->hit(ray, t_min, closest_sofar, temp_hit))
		{
			hit_any = true;
			closest_sofar = temp_hit.t;
		}
	} // end for 

	if (hit_any) {
		outHit = temp_hit;
	}
	return hit_any;
}

bool FHittableAggregate::bounding_box(double t0, double t1, FAABB& outbox) const
{
	if (objects.empty()) return false;

	FAABB temp_box;
	bool first_box = true;

	for (const auto &object : objects)
	{
		if (!object->bounding_box(t0, t1, temp_box)) return false;
	
		outbox = first_box ? temp_box : surrounding_box(outbox, temp_box);
		first_box = false;
	} // end for 

	return true;
}
