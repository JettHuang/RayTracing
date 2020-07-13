// box shape
//
//

#pragma once

#include "aarect.h"
#include "hittable_list.h"


// box
class FBox : public FHittable
{
public:
	FBox() {}
	FBox(const FPoint3& p0, const FPoint3& p1, const shared_ptr<FMaterial>& ptr);

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		outbox = FAABB(box_min, box_max);
		return true;
	}

protected:
	FPoint3 box_min;
	FPoint3 box_max;
	FHittableList sides;
};
