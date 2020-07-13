//
//

#include "hittable.h"


FRotateY::FRotateY(const shared_ptr<FHittable>& p, double angle)
	: ptr(p)
{
	auto radians = degrees_2_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	bHasbox = ptr->bounding_box(0, 1, bbox);

	FPoint3 min(kInfinity, kInfinity, kInfinity);
	FPoint3 max(-kInfinity, -kInfinity, -kInfinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				FVec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = FAABB(min, max);
}

bool FRotateY::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	const FPoint3 &origin = ray.Origin();
	const FVec3 &direction = ray.Direction();
	FPoint3 local_origin = origin;
	FVec3 local_dir = direction;

	local_origin[0] = cos_theta * origin[0] - sin_theta * origin[2];
	local_origin[2] = sin_theta * origin[0] + cos_theta * origin[2];

	local_dir[0] = cos_theta * direction[0] - sin_theta * direction[2];
	local_dir[2] = sin_theta * direction[0] + cos_theta * direction[2];

	FRay rotated_r(local_origin, local_dir, ray.Time());

	if (!ptr->hit(rotated_r, t_min, t_max, outHit))
		return false;

	FPoint3 p = outHit.p;
	FVec3 normal = outHit.normal;

	p[0] = cos_theta * outHit.p[0] + sin_theta * outHit.p[2];
	p[2] = -sin_theta * outHit.p[0] + cos_theta * outHit.p[2];

	normal[0] = cos_theta * outHit.normal[0] + sin_theta * outHit.normal[2];
	normal[2] = -sin_theta * outHit.normal[0] + cos_theta * outHit.normal[2];

	outHit.p = p;
	outHit.normal = normal;

	return true;
}
