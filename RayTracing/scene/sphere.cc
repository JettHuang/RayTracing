// sphere
//
//

#include "sphere.h"


bool FSphere::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	FVec3 oc = ray.Origin() - center;
	auto a = ray.Direction().length2();
	auto half_b = dot(oc, ray.Direction());
	auto c = oc.length2() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant > 0.0) {
		auto root = sqrt(discriminant);
		auto time = 0.0;
		
		// check root1
		auto root1 = (-half_b - root) / a;
		if (root1 < t_max && root1 > t_min) {
			time = root1;
		}
		else {
			// check root2
			auto root2 = (-half_b + root) / a;
			if (root2 < t_max && root2 > t_min) {
				time = root2;
			}
			else {
				return false;
			}
		}

		outHit.t = time;
		outHit.p = ray.At(time);
		FVec3 outward_normal = (outHit.p - center) / radius;
		outHit.set_face_normal(ray, outward_normal);
		get_shere_uv(outward_normal, outHit.u, outHit.v);
		outHit.mat_ptr = mat_ptr;
		return true;
	}

	return false;
}

void get_shere_uv(const FVec3& p, double& u, double& v)
{
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());
	u = 1.0 - (phi + kPi) / kTwoPi;
	v = (theta + kHalfPi) / kPi;
}
