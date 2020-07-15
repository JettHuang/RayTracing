// vector 3d
//
//

#pragma once

#include <iostream>
#include "basic.h"


class FVec3 
{
public:
	FVec3() : e{ 0.0, 0.0, 0.0 } {}
	FVec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	inline double x() const { return e[0]; }
	inline double y() const { return e[1]; }
	inline double z() const { return e[2]; }

	inline FVec3 operator-() const { return FVec3(-e[0], -e[1], -e[2]); }
	inline double operator[](int i) const { return e[i]; }
	inline double& operator[](int i) { return e[i]; }

	FVec3& operator +=(const FVec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];

		return *this;
	}

	FVec3& operator -=(const FVec3& v)
	{
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		e[2] -= v.e[2];

		return *this;
	}

	FVec3& operator *=(const double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	FVec3& operator /=(const double t)
	{
		return *this *= 1.0 / t;
	}

	inline double length() const 
	{
		return sqrt(length2());
	}

	inline double length2() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline static FVec3 random()
	{
		return FVec3(random_double(), random_double(), random_double());
	}

	inline static FVec3 random(double min, double max)
	{
		return FVec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

public:
	double e[3];
};

// type aliases for FVec3
using FPoint3 = FVec3;
using FColor3 = FVec3;

// vec3 utility functions
inline std::ostream& operator<<(std::ostream &out, const FVec3 &v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline FVec3 operator +(const FVec3& lhs, const FVec3& rhs)
{
	return FVec3(lhs.e[0] + rhs.e[0], lhs.e[1] + rhs.e[1], lhs.e[2] + rhs.e[2]);
}

inline FVec3 operator -(const FVec3& lhs, const FVec3& rhs)
{
	return FVec3(lhs.e[0] - rhs.e[0], lhs.e[1] - rhs.e[1], lhs.e[2] - rhs.e[2]);
}

inline FVec3 operator *(const FVec3& lhs, const FVec3& rhs)
{
	return FVec3(lhs.e[0] * rhs.e[0], lhs.e[1] * rhs.e[1], lhs.e[2] * rhs.e[2]);
}

inline FVec3 operator *(double t, const FVec3& v)
{
	return FVec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline FVec3 operator *(const FVec3& v, double t)
{
	return t * v;
}

inline FVec3 operator /(const FVec3& v, double t)
{
	return (1.0 / t) * v;
}

inline double dot(const FVec3& lhs, const FVec3& rhs)
{
	return lhs.e[0] * rhs.e[0]
		+ lhs.e[1] * rhs.e[1]
		+ lhs.e[2] * rhs.e[2];
}

inline FVec3 cross(const FVec3& lhs, const FVec3& rhs)
{
	return FVec3(lhs.e[1] * rhs.e[2] - lhs.e[2] * rhs.e[1],
				 lhs.e[2] * rhs.e[0] - lhs.e[0] * rhs.e[2],
				 lhs.e[0] * rhs.e[1] - lhs.e[1] * rhs.e[0]);
}


inline FVec3 unit_vector(const FVec3& v)
{
	return v / v.length();
}

inline FVec3 random_in_unit_disk() 
{
	while (true)
	{
		auto p = FVec3(random_double(-1, 1), random_double(-1, 1), 0.0);
		if (p.length2() >= 1.0) continue;

		return p;
	}
}

inline FVec3 random_unit_vector()
{
	auto a = random_double(0, 2 * kPi);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);

	return FVec3(r * cos(a), r * sin(a), z);
}

inline FVec3 random_in_unit_sphere()
{
	while (true)
	{
		auto p = FVec3::random(-1.0, 1.0);
		if (p.length2() >= 1.0) continue;
		return p;
	}
}

inline FVec3 random_in_hemisphere(const FVec3& normal) {
	FVec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) >= 0.0)
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

// v is incidence vector
inline FVec3 reflect(const FVec3& v, const FVec3& n)
{
	return v - 2 * dot(v, n) * n;
}

//           R 
//            \  |N
//             \ |       eta
//              \|            
//   ------------+---------------
//               |\        
//               | \     eta'
//               |  R'
//
inline FVec3 refract(const FVec3& uv, const FVec3& n, double etai_over_etat)
{
	auto cos_theta = dot(-uv, n);
	FVec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	FVec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length2()) * n;
	return r_out_parallel + r_out_perp;
}

