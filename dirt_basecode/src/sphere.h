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

#include "surface.h"

//! A sphere centered at the origin with radius m_radius
class Sphere : public Surface
{
public:
	Sphere(float radius = 1.f,
		   const Material * material = Material::defaultMaterial(),
		   const Transform & xform = Transform());
    Sphere(const Scene & scene, const json & j = json::object());
    virtual AABB3f localBBox() const;
    virtual bool intersect(const Ray3f &ray, HitInfo &hit) const;

protected:
    float m_radius = 1.0f;
};