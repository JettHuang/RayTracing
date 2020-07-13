// hittables aggregate
//
//

#pragma once

#include <memory>
#include <vector>

#include "hittable.h"


// hittables aggregate
class FHittableAggregate : public FHittable
{
public:
	FHittableAggregate() {}
	FHittableAggregate(const shared_ptr<FHittable>& obj)
	{
		add(obj);
	}

	void clear() { objects.clear(); }
	void add(const shared_ptr<FHittable>& obj) { objects.push_back(obj); }

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const override;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const override;

public:
	std::vector<shared_ptr<FHittable>> objects;
};
