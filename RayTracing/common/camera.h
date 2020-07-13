// camera.h
//                    |+y
//                   \|
//             <------O------- -z
//                    |\
//                    | \+x
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "ray.h"
#include "plane.h"


// pin-hole camera
class FCamera
{
public:
	FCamera(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double film_dist)
	{
		auto theta = degrees_2_radians(vfov);
		auto h = tan(theta * 0.5) * film_dist;
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;
		
		auto w = unit_vector(lookfrom - lookat);
		auto u = unit_vector(cross(vup, w));
		auto v = cross(w, u);

		origin = lookfrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner = origin - (horizontal * 0.5) - (vertical * 0.5) - w * film_dist;
	}

	virtual FRay castRay(double u, double v) {
		return FRay(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

protected:
	FVec3	origin;
	FVec3	lower_left_corner;
	FVec3	horizontal;  // virtual viewport x
	FVec3	vertical;    // virtual viewport y
};

class FCameraWithShuter : public FCamera
{
public:
	FCameraWithShuter(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double film_dist,
		double t0,  // shutter open timestamp
		double t1)  // shutter close timestamp
		: FCamera(lookfrom, lookat, vup, vfov, aspect_ratio, film_dist)
	{
		time0 = t0;
		time1 = t1;
	}

	virtual FRay castRay(double u, double v) 
	{
		auto timestamp = random_double(time0, time1);
		return FRay(origin, lower_left_corner + u * horizontal + v * vertical - origin, timestamp);
	}

protected:
	double time0;
	double time1;
};

// camera with lens(simulation)
class FCameraWithLens
{
public:
	FCameraWithLens(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double aperture,
		double focus_dist)
	{
		auto theta = degrees_2_radians(vfov);
		auto h = tan(theta * 0.5) * focus_dist;
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner = origin - (horizontal * 0.5) - (vertical * 0.5) - w * focus_dist;

		lens_radius = aperture * 0.5;
	}

	virtual FRay castRay(double s, double t) {
		FVec3 rd = lens_radius * random_in_unit_disk();
		FVec3 offset = u * rd.x() + v * rd.y();

		return FRay(origin + offset, 
			lower_left_corner + s * horizontal + t * vertical - origin -offset);
	}

private:
	FVec3	origin;
	FVec3	lower_left_corner;
	FVec3	horizontal;  // virtual viewport x
	FVec3	vertical;    // virtual viewport y
	FVec3	u, v, w;
	double	lens_radius;
};

// camera with physical thin lens
// 1/f = 1/zi + 1/zo
class FCameraWithPhysicalLens
{
public:
	FCameraWithPhysicalLens(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double aperture,
		double focus_length,
		double image_dist,
		double photo_height,
		double coc_pixel)
	{
		auto theta = degrees_2_radians(vfov);
		auto h = tan(theta * 0.5) * image_dist;
		auto film_height = 2.0 * h;
		auto film_width = aspect_ratio * film_height;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = film_width * u;
		vertical = film_height * v;
		lower_left_corner = origin - (horizontal * 0.5) - (vertical * 0.5) + w * image_dist;

		// zo = (f * zi) / (zi - f)
		double zo = (focus_length * image_dist) / (image_dist - focus_length);
		FVec3 object_plane_loc = origin - zo * w;
		object_plane = FPlane(w, object_plane_loc);

		lens_radius = aperture * 0.5;

		// calculate depth of field (reference GAMES101_Lecture_19.pdf)
		{
			// calculate circle of confusion
			double coc = film_height * coc_pixel / photo_height;
			double Num = focus_length / aperture;
			double f2 = focus_length * focus_length;
			double Dsf2 = zo * f2;
			double NCDsmiusF = Num * coc * (zo - focus_length);

			double Df = Dsf2 / (f2 - NCDsmiusF);
			double Dn = Dsf2 / (f2 + NCDsmiusF);

			depthFieldFar = Df;
			depthFieldNear = Dn;
		}
	}

	virtual FRay castRay(double s, double t)
	{
		// flip s, t
		s = 1.0 - s;
		t = 1.0 - t;

		FVec3 rd = lens_radius * random_in_unit_disk();
		FVec3 offset = u * rd.x() + v * rd.y();

		FPoint3 p0 = lower_left_corner + s * horizontal + t * vertical;
		FPoint3 p1 = origin + offset;
		FPoint3 p2;
		if (!object_plane.interset(FRay(p0, origin-p0), p2))
		{
			return FRay(p0, origin - p0);
		}

		return FRay(p1, p2 - p1);
	}

	void GetDepthOfField(double& Far, double& Near)
	{
		Far = depthFieldFar;
		Near = depthFieldNear;
	}

protected:
	FVec3	origin;
	FVec3	lower_left_corner;
	FVec3	horizontal;  // film x
	FVec3	vertical;    // film y
	FVec3	u, v, w;
	FPlane	object_plane;
	double	lens_radius;
	// depth of field
	double	depthFieldFar;
	double  depthFieldNear;
};

// lens camera with shutter time
class FCameraWithLensShutter : public FCameraWithPhysicalLens
{
public:
	FCameraWithLensShutter(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double aperture,
		double focus_length,
		double image_dist,
		double photo_height,
		double coc_pixel,
		double t0,  // shutter open timestamp
		double t1)  // shutter close timestamp
		: FCameraWithPhysicalLens(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_length, image_dist, photo_height, coc_pixel)
	{
		time0 = t0;
		time1 = t1;
	}

	virtual FRay castRay(double s, double t) 
	{
		auto timestamp = random_double(time0, time1);

		// flip s, t
		s = 1.0 - s;
		t = 1.0 - t;

		FVec3 rd = lens_radius * random_in_unit_disk();
		FVec3 offset = u * rd.x() + v * rd.y();

		FPoint3 p0 = lower_left_corner + s * horizontal + t * vertical;
		FPoint3 p1 = origin + offset;
		FPoint3 p2;
		if (!object_plane.interset(FRay(p0, origin - p0), p2))
		{
			return FRay(p0, origin - p0, timestamp);
		}

		return FRay(p1, p2 - p1, timestamp);
	}

private:
	double	time0;
	double	time1;
};
