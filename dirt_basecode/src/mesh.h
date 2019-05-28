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

    ------------------------------------------------------------------------

    This file is based on the Mesh class from Nori:

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include "surface.h"

/*!
    @brief A triangle mesh.

    This class stores a triangle mesh object and provides numerous functions
    for querying the individual triangles. Subclasses of @c Mesh implement
    the specifics of how to create its contents (e.g. by loading from an
    external file)
 */
struct Mesh
{
public:

	bool empty() const {return F.empty() || V.empty();}

    std::vector<Point3f> V;           //!< Vertex positions
    std::vector<Normal3f> N;          //!< Vertex normals
    std::vector<Point2f> UV;          //!< Vertex texture coordinates
    std::vector<Vector3i> F;          //!< Faces
};


//! An instance of a triangle for a given face in a mesh
class Triangle : public Surface
{
public:
	Triangle(const Scene & scene, const json & j, Mesh * mesh, int triNumber);
	AABB3f localBBox() const override;
	AABB3f worldBBox() const override;
	bool intersect(const Ray3f &ray, HitInfo &hit) const override;

protected:
	Mesh * m_mesh;
	const Vector3i * m_face;
};


// ray - single triangle intersection routine
bool singleTriangleIntersect(const Ray3f& ray,
                             const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
                             const Normal3f* n0, const Normal3f* n1, const Normal3f* n2,
							 const Point2f* uv0, const Point2f* uv1, const Point2f* uv2,
							 HitInfo& isect,
                             const Material * material = nullptr,
                             const Surface * surface = nullptr);

