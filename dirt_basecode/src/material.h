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

#pragma once

#include "fwd.h"
#include "parser.h"
#include "color.h"
#include "rand.h"
#include <stdlib.h>

//! A base class used to represent surface material properties.
class Material
{
public:
	virtual ~Material() = default;

	//! Return a pointer to a global default material
	static const Material * defaultMaterial();

	/*!
	 * @brief Compute the scattered direction scattered at a surface hitpoint.
	 *
	 * The base Material does not scatter any light, so it simply returns false.
	 *
	 * @param[in] 	ray			incoming ray
	 * @param[in] 	hit			the ray's intersection with the surface
	 * @param[out] 	attenuation	how much the light should be attenuated
	 * @param[out] 	scattered	the direction light should be scattered
	 * @return					True if the surface scatters light
	 */
	virtual bool scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const
	{
		return false;
	}

	/*!
	 * @brief Compute the amount of emitted light at the surface hitpoint.
	 *
	 * The base Material class does not emit any light, so it simply returns black.
	 *
	 * @param[in] ray	the incoming ray
	 * @param[in] hit 	the ray's intersection with the surface
	 * @return 			The emitted color
	 */
	virtual Color3f emitted(const Ray3f &ray, const HitInfo &hit) const
	{
		return Color3f(0,0,0);
	}
};


//! A perfectly diffuse (Lambertian) material
class Lambertian : public Material
{
public:
	Lambertian(const json & j = json::object(), const Scene* scene = nullptr);

	bool scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const override;

	Texture* albedo;				   //!< The diffuse color (fraction of light that is reflected per color channel).
};


//! A metallic material that reflects light into the (potentially rough) mirror reflection direction.
class Metal : public Material
{
public:
	Metal(const json & j = json::object(), const Scene* scene = nullptr);

	bool scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const override;

	Texture* albedo;					//!< The reflective color (fraction of light that is reflected per color channel).
	Texture* roughness;					//!< A value between 0 and 1 indicating how smooth vs. rough the reflection should be.
};


//! A smooth dielectric surface that reflects and refracts light according to the specified index of refraction #ior
class Dielectric : public Material
{
public:
	Dielectric(const json & j = json::object(), const Scene* scene = nullptr);

	bool scatter(const Ray3f &ray, const HitInfo &hit, Color3f &attenuation, Ray3f &scattered) const override;

	float ior;      //!< The (relative) index of refraction of the material
};


//! A material that emits light equally in all directions from the front side of a surface.
class DiffuseLight : public Material
{
public:
	DiffuseLight(const json & j = json::object(), const Scene* scene = nullptr);

	//! Returns a constant Color3f if the ray hits the surface on the front side.
	Color3f emitted(const Ray3f &ray, const HitInfo &hit) const override;

	Texture* emit;	//!< The emissive color of the light
};