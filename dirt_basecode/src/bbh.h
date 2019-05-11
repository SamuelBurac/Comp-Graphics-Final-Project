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

#include "surfacegroup.h"

class BBHNode : public Surface
{
    AABB3f m_bounds;
    Surface *m_left;
    Surface *m_right;

public:
    BBHNode(std::vector<Surface *> primitives);
    ~BBHNode();

    bool intersect(const Ray3f &ray, HitInfo &hit) const;

    AABB3f localBBox() const override { return m_bounds; }
    AABB3f worldBBox() const override { return m_bounds; }
};

//! An axis-aligned bounding box hierarchy acceleration structure
class BBH: public SurfaceGroup
{
    BBHNode *m_root;

public:
    BBH(const Scene & scene, const json & j = json::object());
    ~BBH() override;

    //! Construct the BBH (must be called before @ref intersect)
    void build() override;

    //! Intersect a ray against all surfaces registered with the Accelerator
    bool intersect(const Ray3f &ray, HitInfo &hit) const override;
};

/*!
 * @brief           Compute the intersection of a Ray with an AABB
 * @param bounds	The AABB to intersect
 * @param ray 		The ray along which to find the intersections
 * @return 			@c true if there is an intersection
 */
bool aabbIntersect(const AABB3f &bounds, const Ray3f &ray);
