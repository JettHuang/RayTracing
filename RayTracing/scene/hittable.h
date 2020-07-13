// hittable interface
//
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"


class FMaterial;

struct FHitRecord
{
	FPoint3	p;
	FVec3	normal;
	shared_ptr<FMaterial> mat_ptr;
	double  t;
	double u;   // texture coordination <u,v>
	double v;
	bool	front_face;

	inline void set_face_normal(const FRay& ray, const FVec3& outward_normal)
	{
		front_face = dot(ray.Direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

// abstract hittable
class FHittable 
{
public:
	virtual bool hit(const FRay &ray, double t_min, double t_max, FHitRecord &outHit) const = 0;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const = 0;
};

// flip face(normal)
class FFlipFace : public FHittable
{
public:
	FFlipFace(const shared_ptr<FHittable> &p) : ptr(p) {}

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
	{
		if (!ptr->hit(ray, t_min, t_max, outHit))
			return false;

		outHit.front_face = !outHit.front_face;
		return true;
	}

	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		return ptr->bounding_box(t0, t1, outbox);
	}

protected:
	shared_ptr<FHittable> ptr;
};

// translate
class FTranslate : public FHittable
{
public:
	FTranslate(const shared_ptr<FHittable>& p, const FVec3 &displacement) 
		: ptr(p), offset(displacement) {}

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
	{
		FRay local_ray(ray.Origin() - offset, ray.Direction(), ray.Time());

		if (!ptr->hit(local_ray, t_min, t_max, outHit))
			return false;

		outHit.p += offset;
		return true;
	}

	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		if (!ptr->bounding_box(t0, t1, outbox))
			return false;

		outbox = FAABB(outbox.min() + offset, outbox.max() + offset);
		return true;
	}

protected:
	shared_ptr<FHittable> ptr;
	FVec3	offset;
};

// rotation around Y
class FRotateY : public FHittable
{
public:
	FRotateY(const shared_ptr<FHittable>& p, double angle);

	virtual bool hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const;
	virtual bool bounding_box(double t0, double t1, FAABB& outbox) const
	{
		outbox = bbox;
		return bHasbox;
	}

protected:
	shared_ptr<FHittable> ptr;
	double sin_theta;
	double cos_theta;
	FAABB bbox;
	bool bHasbox;
};

