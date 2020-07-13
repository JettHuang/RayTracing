// axies-aligned rectangle
//
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "hittable.h"


class FXYRect : public FHittable {
public:
	FXYRect() {}

	FXYRect(
		double _x0, double _x1, double _y0, double _y1, double _k, const shared_ptr<FMaterial> &mat
	) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

	virtual bool hit(const FRay& r, double t0, double t1, FHitRecord& rec) const;

	virtual bool bounding_box(double t0, double t1, FAABB& output_box) const {
		// The bounding box must have non-zero width in each dimension, so pad the Z
		// dimension a small amount.
		output_box = FAABB(FPoint3(x0, y0, k - 0.0001), FPoint3(x1, y1, k + 0.0001));
		return true;
	}

public:
	shared_ptr<FMaterial> mp;
	double x0, x1, y0, y1, k;
};

class FXZRect : public FHittable {
public:
	FXZRect() {}

	FXZRect(
		double _x0, double _x1, double _z0, double _z1, double _k, const shared_ptr<FMaterial>& mat
	) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const FRay& r, double t0, double t1, FHitRecord& rec) const;

	virtual bool bounding_box(double t0, double t1, FAABB& output_box) const {
		// The bounding box must have non-zero width in each dimension, so pad the Y
		// dimension a small amount.
		output_box = FAABB(FPoint3(x0, k - 0.0001, z0), FPoint3(x1, k + 0.0001, z1));
		return true;
	}

public:
	shared_ptr<FMaterial> mp;
	double x0, x1, z0, z1, k;
};

class FYZRect : public FHittable {
public:
	FYZRect() {}

	FYZRect(
		double _y0, double _y1, double _z0, double _z1, double _k, const shared_ptr<FMaterial>& mat
	) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const FRay& r, double t0, double t1, FHitRecord& rec) const;

	virtual bool bounding_box(double t0, double t1, FAABB& output_box) const {
		// The bounding box must have non-zero width in each dimension, so pad the X
		// dimension a small amount.
		output_box = FAABB(FPoint3(k - 0.0001, y0, z0), FPoint3(k + 0.0001, y1, z1));
		return true;
	}

public:
	shared_ptr<FMaterial> mp;
	double y0, y1, z0, z1, k;
};

