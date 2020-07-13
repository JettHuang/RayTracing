// ray (origin + direction)
//
//

#pragma once

#include "vec3.h"


class FRay
{
public:
	FRay() : orig(0,0,0), dir(0,0,-1), tm(0) 
	{}
	FRay(const FPoint3& origin, const FVec3& direction)
		: orig(origin), dir(direction), tm(0)
	{}

	FRay(const FPoint3& origin, const FVec3& direction, double time)
		: orig(origin), dir(direction), tm(time)
	{}

	inline const FPoint3& Origin() const { return orig; }
	inline const FVec3& Direction() const { return dir; }
	inline double Time() const { return tm; }


	inline FPoint3 At(double t) const {
		return orig + t * dir;
	}

public:
	FPoint3	orig;
	FVec3	dir;
	double tm;
};