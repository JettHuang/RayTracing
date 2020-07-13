// hittable: moving sphere
//
//

#pragma once

#include "hittable.h"

class FMaterial;

// position track key frame
struct FPositionTrackKey
{
	FPositionTrackKey()
		: pos(0,0,0)
		, time(0)
	{}

	FPositionTrackKey(const FPoint3 &p, double t)
		: pos(p)
		, time(t)
	{}

	FPoint3	pos;
	double	time;
};

// moving sphere
class FMovingSphere : public FHittable
{
public:
	FMovingSphere() : radius(0) {}
	FMovingSphere(const FPositionTrackKey &k0, const FPositionTrackKey& k1, double InRadius, const shared_ptr<FMaterial> &m)
		: key0(k0)
		, key1(k1)
		, radius(InRadius)
		, mat_ptr(m)
	{}

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const override;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const override;

	FPoint3 Position(double time) const;

public:
	FPositionTrackKey key0;
	FPositionTrackKey key1;

	double		radius;
	shared_ptr<FMaterial>	mat_ptr;

};
