// bounding volume hierarchy
//
//

#pragma once

#include "basic.h"
#include "hittable.h"
#include "hittable_aggregate.h"


#define MAX_HITTABLES_IN_LEAF	5


// bvh node
class FBVH_Node : public FHittable
{
public:
	FBVH_Node();

	FBVH_Node(FHittableAggregate &list, double time0, double time1)
		: FBVH_Node(list.objects, 0, list.objects.size(), time0, time1)
	{}

	// [start, end)
	FBVH_Node(std::vector<shared_ptr<FHittable>>& objects, size_t start, size_t end, double time0, double time1);

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		outbox = box;
		return true;
	}

public:
	shared_ptr<FHittable> left;
	shared_ptr<FHittable> right;
	FAABB box;
};
