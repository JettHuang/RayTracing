// constant medium

#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"


// constant medium
class FConstantMedium : public FHittable
{
public:
	FConstantMedium(const shared_ptr<FHittable>& b, double density, const shared_ptr<FTexture>& a)
		: boundary(b)
		, neg_inv_density(-1.0 / density)
	{
		phase_function = make_shared<FIsotropic>(a);
	}

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& rec) const
	{
		FHitRecord rec1, rec2;

		if (!boundary->hit(ray, -kInfinity, kInfinity, rec1))
			return false;

		if (!boundary->hit(ray, rec1.t + 0.0001, kInfinity, rec2))
			return false;

		if (rec1.t < t_min) rec1.t = t_min;
		if (rec2.t > t_max) rec2.t = t_max;

		if (rec1.t >= rec2.t)
			return false;

		if (rec1.t < 0.0) rec1.t = 0.0;

		const auto ray_length = ray.Direction().length();
		const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
		const auto hit_distance = neg_inv_density * log(random_double());

		if (hit_distance > distance_inside_boundary)
			return false;

		rec.t = rec1.t + hit_distance / ray_length;
		rec.p = ray.At(rec.t);

		rec.normal = FVec3(1, 0, 0);  // arbitrary
		rec.front_face = true;     // also arbitrary
		rec.mat_ptr = phase_function;

		return true;
	}

	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		return boundary->bounding_box(t0, t1, outbox);
	}

protected:
	shared_ptr<FHittable> boundary;
	shared_ptr<FMaterial> phase_function;
	double neg_inv_density;
};
