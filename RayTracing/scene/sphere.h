// hittable: sphere
//
//

#pragma once

#include "hittable.h"

class FMaterial;

// sphere
class FSphere : public FHittable
{
public:
	FSphere() : center(0,0,0), radius(0) {}
	FSphere(const FPoint3 &InCenter, double InRadius, const shared_ptr<FMaterial> &m)
		: center(InCenter)
		, radius(InRadius)
		, mat_ptr(m)
	{}

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const override;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const override
	{
		outbox = FAABB(center - FVec3(radius, radius, radius),
			center + FVec3(radius, radius, radius));
		return true;
	}

public:
	FPoint3		center;
	double		radius;
	shared_ptr<FMaterial>	mat_ptr;
};


// caculate uv of sphere (p in a point on the unit sphere)
void get_shere_uv(const FVec3& p, double& u, double& v);
