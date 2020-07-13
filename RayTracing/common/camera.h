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


// abstract camera
class FRayCamera
{
public:
	virtual FRay castRay(double u, double v) const = 0;
};


// pin-hole camera
class FPinholeCamera : public FRayCamera
{
public:
	FPinholeCamera(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double film_dist,
		double t0,  // shutter open timestamp
		double t1)  // shutter close timestamp
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

		time0 = t0;
		time1 = t1;
	}

	virtual FRay castRay(double u, double v) const override 
	{
		auto timestamp = random_double(time0, time1);
		return FRay(origin, lower_left_corner + u * horizontal + v * vertical - origin, timestamp);
	}

protected:
	FVec3	origin;
	FVec3	lower_left_corner;
	FVec3	horizontal;  // virtual viewport x
	FVec3	vertical;    // virtual viewport y
	double  time0;
	double  time1;
};


// camera with physical thin lens
// 1/f = 1/zi + 1/zo
class FThinlensCamera : public FRayCamera
{
public:
	FThinlensCamera(
		FPoint3 lookfrom,
		FPoint3	lookat,
		FVec3	vup,
		double vfov, // vertical field-of-view in degree
		double aspect_ratio,
		double aperture,
		double focus_length,
		double film_dist,
		double photo_height,
		double coc_pixel,
		double t0,  // shutter open timestamp
		double t1)  // shutter close timestamp
	{
		auto theta = degrees_2_radians(vfov);
		auto h = tan(theta * 0.5) * film_dist;
		auto film_height = 2.0 * h;
		auto film_width = aspect_ratio * film_height;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = film_width * u;
		vertical = film_height * v;
		lower_left_corner = origin - (horizontal * 0.5) - (vertical * 0.5) + w * film_dist;

		// zo = (f * zi) / (zi - f)
		double zo = (focus_length * film_dist) / (film_dist - focus_length);
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

		time0 = t0;
		time1 = t1;
	}

	virtual FRay castRay(double s, double t) const override
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
		if (!object_plane.interset(FRay(p0, origin-p0), p2))
		{
			return FRay(p0, origin - p0, timestamp);
		}

		return FRay(p1, p2 - p1, timestamp);
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
	double	time0;
	double	time1;

	// depth of field
	double	depthFieldFar;
	double  depthFieldNear;
};
