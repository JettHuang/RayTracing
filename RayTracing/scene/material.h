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
		FVec3 scatter_direction = unit_vector(random_in_hemisphere(rec.normal));
		scattered = FRay(rec.p, scatter_direction, ray_in.Time());

		FColor3 p = albedo->value(rec.u, rec.v, rec.p);
		double cos_theta = std::max(dot(rec.normal, scatter_direction), 0.0);
		attenuation = (kOneOverPi * cos_theta) * p;
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

// pbr material (using Cook-Torrance BRDF)
// https://zhuanlan.zhihu.com/p/152226698
// https://learnopengl-cn.github.io/07%20PBR/02%20Lighting
class FPbrMaterial : public FMaterial
{
public:
	FPbrMaterial(const shared_ptr<FTexture>& InAlbedoTex, const shared_ptr<FTexture>& InMetallicTex, const shared_ptr<FTexture>& InRoughnessTex)
		: albedoTex(InAlbedoTex)
		, metallicTex(InMetallicTex)
		, roughnessTex(InRoughnessTex)
	{}

	virtual bool scatter(const FRay& ray_in, const FHitRecord& rec, FColor3& attenuation, FRay& scattered) const
	{
		FVec3 scatter_direction = unit_vector(random_in_hemisphere(rec.normal));
		scattered = FRay(rec.p, scatter_direction, ray_in.Time());

		const FVec3& N = rec.normal;
		const FVec3& L = scatter_direction;
		const FVec3  V = -unit_vector(ray_in.Direction());
		const FVec3  H = unit_vector(V + L);

		const FColor3 albedo = albedoTex->value(rec.u, rec.v, rec.p);
		const double metallic = metallicTex->value(rec.u, rec.v, rec.p)[0];
		const double roughness = roughnessTex->value(rec.u, rec.v, rec.p)[0];

		// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
		// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)   
		static const FVec3 kFb(0.04, 0.04, 0.04);
		const FVec3 F0 = lerp(kFb, albedo, metallic); // mix

		// Cook-Torrance BRDF
		double NDF = DistributionGGX(N, H, roughness);
		double G = GeometrySmith(N, V, L, roughness);
		FVec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		FVec3 nominator = NDF * G * F;
		double denominator = 4.0 * std::max(dot(N, V), 0.0) * std::max(dot(N, L), 0.0);
		const FColor3 brdf_specular = nominator / std::max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0


		// kS is equal to Fresnel
		FVec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		FVec3 kD = FVec3(1.0,1.0,1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		const FColor3 brdf_diffuse = albedo * kOneOverPi; // lambertian model
		const FColor3 brdf_cooktorrance = kD * brdf_diffuse + brdf_specular; // kS is Fresnel

		const double NdotL = std::max(dot(N, L), 0.0);
		attenuation = brdf_cooktorrance * NdotL;
	}

protected:
	double DistributionGGX(const FVec3& N, const FVec3& H, double InRoughness) const
	{
		double a = InRoughness * InRoughness;
		double a2 = a * a;
		double NdotH = std::max(dot(N, H), 0.0);
		double NdotH2 = NdotH * NdotH;

		double nom = a2;
		double denom = NdotH2 * (a2 - 1.0) + 1.0;
		denom = kPi * denom * denom;

		return nom / std::max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
	}

	double GeometrySchlickGGX(double NdotV, double InRoughness) const
	{
		double r = InRoughness + 1.0;
		double k = (r * r) * (1.0 / 8.0);
		double nom = NdotV;
		double denom = NdotV * (1.0 - k) + k;

		return nom / denom;
	}

	double GeometrySmith(const FVec3& N, const FVec3& V, const FVec3 &L, double InRoughness) const
	{
		double NdotV = std::max(dot(N, V), 0.0);
		double NdotL = std::max(dot(N, L), 0.0);
		double ggx2 = GeometrySchlickGGX(NdotV, InRoughness);
		double ggx1 = GeometrySchlickGGX(NdotL, InRoughness);

		return ggx1 * ggx2;
	}

	FVec3 fresnelSchlick(double HdotV, const FVec3& F0) const
	{
		return F0 + (FVec3(1.0,1.0,1.0) - F0) * pow(1.0 - HdotV, 5.0);
	}

protected:
	shared_ptr<FTexture> albedoTex;
	shared_ptr<FTexture> metallicTex;
	shared_ptr<FTexture> roughnessTex;
};
