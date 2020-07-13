// axies-aligned bounding box
//
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "ray.h"


#define AABB_X		0
#define AABB_Y		1
#define AABB_Z		2

class FAABB
{
public:
	FAABB() {}
	FAABB(const FPoint3 &a, const FPoint3 &b) 
		: _min(a), _max(b)
	{}

	const FPoint3& min() const { return _min; }
	const FPoint3& max() const { return _max; }

	bool hit(const FRay& ray, double tmin, double tmax) const
	{
		FPoint3 origin = ray.Origin();
		FVec3 dir = ray.Direction();

		for (int a=0; a<3; a++)
		{
			auto t0 = fmin((_min[a] - origin[a]) / dir[a],
						   (_max[a] - origin[a]) / dir[a]);
			auto t1 = fmax((_min[a] - origin[a]) / dir[a],
						   (_max[a] - origin[a]) / dir[a]);
			tmin = fmax(t0, tmin);
			tmax = fmin(t1, tmax);
			if (tmax <= tmin)
				return false;
		} // end for a

		return true;
	}

	// area of surface
	double area() const 
	{
		auto a = _max.x() - _min.x();
		auto b = _max.y() - _min.y();
		auto c = _max.z() - _min.z();
		return 2.0 * (a * b + b * c + c * a);
	}

	int longest_axies() const
	{
		auto a = _max.x() - _min.x();
		auto b = _max.y() - _min.y();
		auto c = _max.z() - _min.z();
		if (a > b && a > c)
		{
			return AABB_X;
		}
		else if (b > c)
		{
			return AABB_Y;
		}

		return AABB_Z;
	}

protected:
	FPoint3	_min;
	FPoint3	_max;
};

inline FAABB surrounding_box(const FAABB& box0, const FAABB& box1)
{
	const FVec3& min0 = box0.min();
	const FVec3& max0 = box0.max();
	const FVec3& min1 = box1.min();
	const FVec3& max1 = box1.max();

	FVec3 small(fmin(min0.x(), min1.x()),
		        fmin(min0.y(), min1.y()),
		        fmin(min0.z(), min1.z()));
	FVec3 big(fmax(max0.x(), max1.x()),
			  fmax(max0.y(), max1.y()),
		      fmax(max0.z(), max1.z()));

	return FAABB(small, big);
}

