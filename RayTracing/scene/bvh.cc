// bounding volume hierarchy
//
//

#include "bvh.h"
#include "hittable_list.h"
#include <algorithm>


inline bool box_compare(const shared_ptr<FHittable> &a, const shared_ptr<FHittable> &b, int axis) 
{
	FAABB box_a;
	FAABB box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis];
}


bool box_x_compare(const shared_ptr<FHittable> &a, const shared_ptr<FHittable> &b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<FHittable> &a, const shared_ptr<FHittable> &b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<FHittable> &a, const shared_ptr<FHittable> &b) {
	return box_compare(a, b, 2);
}


FBVH_Node::FBVH_Node(std::vector<shared_ptr<FHittable>>& objects, size_t start, size_t end, double time0, double time1)
{
	int axis = random_int(0, 2);
	auto comparator = (axis == AABB_X) ? box_x_compare
		: (axis == AABB_Y) ? box_y_compare : box_z_compare;

	size_t object_span = end - start;

	if (object_span <= MAX_HITTABLES_IN_LEAF)
	{
		shared_ptr<FHittableList> hitablelist = make_shared<FHittableList>();
		for (size_t i = start; i < end; i++)
		{
			hitablelist->add(objects[i]);
		}
		left = hitablelist;
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		left = make_shared<FBVH_Node>(objects, start, mid, time0, time1);
		right = make_shared<FBVH_Node>(objects, mid, end, time0, time1);
	}

	FAABB box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left)
		|| (right && !right->bounding_box(time0, time1, box_right))
		)
	{
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	if (right)
	{
		box = surrounding_box(box_left, box_right);
	}
	else
	{
		box = box_left;
	}
}
	

bool FBVH_Node::hit(const FRay& ray, double t_min, double t_max, FHitRecord& outHit) const
{
	if (!box.hit(ray, t_min, t_max))
		return false;

	bool hit_left = left->hit(ray, t_min, t_max, outHit);
	bool hit_right = right ? right->hit(ray, t_min, (hit_left ? outHit.t : t_max), outHit) : false;

	return hit_left || hit_right;
}
