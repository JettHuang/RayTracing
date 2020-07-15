// moving sphere
//
//

#include "moving_sphere.h"

void get_shere_uv(const FVec3& p, double& u, double& v);

bool FMovingSphere::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	const FPoint3 center = Position(ray.Time());

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

FPoint3 FMovingSphere::Position(double time) const
{
	double t = (time - key0.time) / (key1.time - key0.time);
	return lerp(key0.pos, key1.pos, t);
}

bool FMovingSphere::bounding_box(double t0, double t1, FAABB& outbox) const
{
	FVec3 bound(radius, radius, radius);
	FPoint3 center0 = Position(t0);
	FPoint3 center1 = Position(t1);

	FAABB box0(center0 - bound, center0 + bound);
	FAABB box1(center1 - bound, center1 + bound);

	outbox = surrounding_box(box0, box1);
	return true;
}
