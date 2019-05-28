/*
    This file is part of Dirt, the Dartmouth introductory ray tracer, used in
    Dartmouth's COSC 77/177 Computer Graphics course.

    Copyright (c) 2018 by Wojciech Jarosz

    Dirt is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Dirt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "material.h"
#include "parser.h"
#include "scene.h"
#include "texture.h"

namespace
{

const Material g_defaultMaterial = Lambertian();

inline float schlickFresnel(float cosine, float ior)
{
	float r0 = (1 - ior) / (1 + ior);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

inline bool refract(const Vector3f &v, const Vector3f &n, float iorIOverT, Vector3f &refracted)
{
	Vector3f uv = v.normalized();
	float dt = uv.dot(n);
	float discrim = 1.0f - iorIOverT * iorIOverT * (1.0f - dt * dt);
	if (discrim > 0)
	{
		refracted = iorIOverT * (uv - n * dt) - n * std::sqrt(discrim);
		return true;
	}
	else
		return false;
}

inline Vector3f reflect(const Vector3f &v, const Vector3f &n)
{
	return v - 2 * v.dot(n) * n;
}

} // namespace


const Material * Material::defaultMaterial()
{
	return &g_defaultMaterial;
}


Lambertian::Lambertian(const json & j, const Scene* scene)
{
	albedo = Texture::parse("albedo", j, Color3f(0.8f), scene);
}

bool Lambertian::scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const
{
	scattered = Ray3f(hit.p, hit.sn + randomInUnitSphere().normalized());
	attenuation = albedo->sample(hit);
	return true;
}


Metal::Metal(const json& j, const Scene* scene)
{
	albedo =	Texture::parse("albedo", j, Color3f(0.8f), scene);
	roughness = Texture::parse("roughness", j, Color3f(0.0f), scene);
}

bool Metal::scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const
{
	Vector3f reflected = reflect(ray.d.normalized(), hit.sn);
	scattered = Ray3f(hit.p, reflected + roughness->sample(hit).r() * randomInUnitSphere().normalized());
	attenuation = albedo->sample(hit);
	return (scattered.d.dot(hit.sn) > 0);
}


Dielectric::Dielectric(const json & j, const Scene* scene)
{
	ior = j.value("ior", ior);
}

bool Dielectric::scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const
{
	// TODO: Implement dielectric scattering
	return false;
}


DiffuseLight::DiffuseLight(const json & j, const Scene* scene)
{
	emit = Texture::parse("emit", j, Color3f(1.0f), scene);
}

Color3f DiffuseLight::emitted(const Ray3f &ray, const HitInfo &hit) const
{
	// only emit from the normal-facing side
	if (ray.d.dot(hit.sn) > 0)
		return Color3f(0,0,0);
	else
		return emit->sample(hit);
}