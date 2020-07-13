// plane
// ie: N is normal of plane, P0 is on plane.
//     N*(P-P0) = 0
//     N*P - N*P0 = 0
//=>   N*P + d = 0

#pragma once

#include "basic.h"
#include "vec3.h"
#include "ray.h"


// plane
class FPlane
{
public:
	FPlane()
		: N(0,0,1)
		, d(0)
	{}

	FPlane(const FVec3& n, const FVec3& p0)
	{
		N = n;
		d = -dot(n, p0);
	}

	bool interset(const FRay& ray, FVec3& out) const
	{
		FVec3 O = ray.Origin();
		FVec3 D = ray.Direction();
		auto dividend = -dot(N, D);
		auto divisor = dot(N, O) + d;
		if (dividend <= KINDA_SMALL_NUMBER && dividend >= -KINDA_SMALL_NUMBER)
		{
			return false;
		}

		auto t = divisor / dividend;
		out = ray.At(t);

		return (t >= 0);
	}

private:
	FVec3	N;
	double	d;
};
