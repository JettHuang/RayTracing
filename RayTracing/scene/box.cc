// box shape
//

#include "box.h"


FBox::FBox(const FPoint3& p0, const FPoint3& p1, const shared_ptr<FMaterial>& ptr)
	: box_min(p0), box_max(p1)
{
	sides.add(make_shared<FXYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(make_shared<FFlipFace>(
		make_shared<FXYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));

	sides.add(make_shared<FXZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	sides.add(make_shared<FFlipFace>(
		make_shared<FXZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));

	sides.add(make_shared<FYZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	sides.add(make_shared<FFlipFace>(
		make_shared<FYZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));
}

bool FBox::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	return sides.hit(ray, t_min, t_max, outHit);
}
