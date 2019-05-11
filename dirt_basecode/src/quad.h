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
#include "surfacegroup.h"

//! A quad spanning (-m_size/2, m_size/2) in the (x,y)-plane at z=0
class Quad : public Surface
{
public:
	Quad(const Vector2f size = Vector2f(1.f),
	     const Material * material = Material::defaultMaterial(),
	     const Transform & xform = Transform());
    Quad(const Scene & scene, const json & j = json::object());
    AABB3f localBBox() const override;
    bool intersect(const Ray3f &ray, HitInfo &hit) const override;

protected:
    Vector2f m_size = Vector2f(1.f);
};