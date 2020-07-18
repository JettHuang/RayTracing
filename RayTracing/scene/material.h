// material
//
//

#pragma once

#include "basic.h"
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "texture.h"


// abstract material
class FMaterial
{
public:
	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const = 0;
	virtual FColor3 emitted(double u, double v, const FPoint3& p) const
	{
		return FColor3(0,0,0);
	}
	virtual double pdf(const FVec3& wi) const
	{
		return 1.0;
	}
};


// lambertian 
class FLambertian : public FMaterial
{
public:
	FLambertian(const shared_ptr<FTexture> &a) : albedo(a) {}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		FVec3 scatter_directin = random_in_hemisphere(rec.normal);
		FColor3 p = albedo->value(rec.u, rec.v, rec.p);
		double cos_theta = dot(rec.normal, scatter_directin);

		attenuation = (kOneOverPi * cos_theta) * p;
		scattered = FRay(rec.p, scatter_directin, ray_in.Time());
		return true;
	}

	virtual double pdf(const FVec3& wi) const
	{
		return 1.0 / kTwoPi; // pdf of hemisphere
	}

public:
	shared_ptr<FTexture>  albedo; // diffuse albedo texture
};


// meta (reflection)
class FMetal : public FMaterial
{
public:
	FMetal(const FColor3& a, double f=1.0) : albedo(a), fuzzy(f<1.0 ? f : 1.0) {}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		FVec3 reflected = reflect(unit_vector(ray_in.Direction()), rec.normal);
		scattered = FRay(rec.p, reflected + fuzzy * random_in_unit_sphere(), ray_in.Time());
		attenuation = albedo;

		return (dot(scattered.Direction(), rec.normal) > 0);
	}

public:
	FColor3 albedo;
	double fuzzy;
};

// Schlick approximation
inline double schlick(double cosine, double ref_idx)
{
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1-cosine, 5);
}

// dielectric
class FDielectric : public FMaterial
{
public:
	FDielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		attenuation = FColor3(1.0, 1.0, 1.0);
		double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : ref_idx;

		FVec3 unit_direction = unit_vector(ray_in.Direction());
		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
		if (etai_over_etat * sin_theta > 1.0) // total internal reflection(È«ÄÚ·´Éä)
		{
			FVec3 reflected = reflect(unit_direction, rec.normal);
			scattered = FRay(rec.p, reflected, ray_in.Time());
			return true;
		}

		// schlick approximation
		double reflect_prob = schlick(cos_theta, etai_over_etat);
		if (random_double() < reflect_prob)
		{
			FVec3 reflected = reflect(unit_direction, rec.normal);
			scattered = FRay(rec.p, reflected, ray_in.Time());
			return true;
		}

		FVec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = FRay(rec.p, refracted, ray_in.Time());
		return true;
	}

private:
	double ref_idx;
};

// diffuse light
class FDiffuseLight : public FMaterial
{
public:
	FDiffuseLight(const std::shared_ptr<FTexture>& a) : emittexture(a) {}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		return false;
	}

	virtual FColor3 emitted(double u, double v, const FPoint3& p) const
	{
		return emittexture->value(u, v, p);
	}

protected:
	shared_ptr<FTexture> emittexture;
};

// isotropic material
class FIsotropic : public FMaterial
{
public:
	FIsotropic(const shared_ptr<FTexture>& a) : albedo(a) {}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		scattered = FRay(rec.p, random_in_unit_sphere(), ray_in.Time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);

		return true;
	}

protected:
	shared_ptr<FTexture> albedo;
};
