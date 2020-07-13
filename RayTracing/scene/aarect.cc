//
//

#include "aarect.h"


bool FXYRect::hit(const FRay& r, double t0, double t1, FHitRecord& rec) const 
{
	const FPoint3 origin = r.Origin();
	const FVec3 direction = r.Direction();

	auto t = (k - origin.z()) / direction.z();
	if (t < t0 || t > t1)
		return false;

	auto x = origin.x() + t * direction.x();
	auto y = origin.y() + t * direction.y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;

	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	auto outward_normal = FVec3(0, 0, 1);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);

	return true;
}

bool FXZRect::hit(const FRay& r, double t0, double t1, FHitRecord& rec) const 
{
	const FPoint3 origin = r.Origin();
	const FVec3 direction = r.Direction();

	auto t = (k - origin.y()) / direction.y();
	if (t < t0 || t > t1)
		return false;

	auto x = origin.x() + t * direction.x();
	auto z = origin.z() + t * direction.z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;

	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = FVec3(0, 1, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);

	return true;
}

bool FYZRect::hit(const FRay& r, double t0, double t1, FHitRecord& rec) const 
{
	const FPoint3 origin = r.Origin();
	const FVec3 direction = r.Direction();

	auto t = (k - origin.x()) / direction.x();
	if (t < t0 || t > t1)
		return false;

	auto y = origin.y() + t * direction.y();
	auto z = origin.z() + t * direction.z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;

	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = FVec3(1, 0, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);

	return true;
}
