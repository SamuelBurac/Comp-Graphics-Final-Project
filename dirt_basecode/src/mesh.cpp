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

#include "mesh.h"

// Ray-Triangle intersection
// p0, p1, p2 - Triangle vertices
// n0, n1, n2 - optional per vertex normal data
bool singleTriangleIntersect(const Ray3f& ray,
	                           const Vector3f& p0, const Vector3f& p1, const Vector3f& p2,
	                           const Normal3f* n0, const Normal3f* n1, const Normal3f* n2,
	                           HitInfo& its,
	                           const Material * material,
	                           const Surface * surface)
{
    // TODO: Implement ray-triangle intersection
    // TODO: If the ray misses the triangle, you should return false
    //       You can pick any ray triangle intersection routine you like.
    //       I recommend you follow the Moller-Trumbore algorithm

    put_your_code_here("Insert your ray-triangle intersection code here");
    return false;

    // First, check for intersection and fill in the hitT
    float hitT = 0.0f;
    // You should also compute the u/v (i.e. the alpha/beta barycentric coordinates) of the hit point
    // (Moller-Trumbore gives you this for free)
    float u, v;

    // TODO: If you successfully hit the triangle, you should check if the hitT lies
    //       within the ray's tmin/tfar, and return false if it does not

    // TODO: Fill in the geometric normal with the geometric normal of the triangle (i.e. normalized cross product of the sides)
    Vector3f gn = Vector3f(0.0f);

    // Compute the shading normal
    Vector3f sn;
    if (n0 != nullptr && n1 != nullptr && n2 != nullptr) { // Do we have per-vertex normals available?
        // We do -> dereference the pointers
        Vector3f normal0 = *n0;
        Vector3f normal1 = *n1;
        Vector3f normal2 = *n2;

        // TODO: You should compute the shading normal by
        //       doing barycentric interpolation of the per-vertex normals (normal0/1/2)
        //       Make sure to normalize the result
        sn = Vector3f(0.0f);
    } else {
        // We don't have per-vertex normals - just use the geometric normal
        sn = gn;
    }

    // Because we've hit the triangle, fill in the intersection data
    its = HitInfo(hitT, ray(hitT), gn, sn, Point2f(u, v), material, surface);
    return true;
}

Triangle::Triangle(const Scene & scene, const json & j, Mesh * mesh, int triNumber)
    : Surface(scene, j), m_mesh(mesh), m_face(&mesh->F[triNumber])
{

}

bool Triangle::intersect(const Ray3f &ray, HitInfo &hit) const
{
    INCREMENT_INTERSECTION_TESTS;

    auto i0 = m_face->x(), i1 = m_face->y(), i2 = m_face->z();
    const Point3f p0 = m_mesh->V[i0], p1 = m_mesh->V[i1], p2 = m_mesh->V[i2];
    const Normal3f * n0 = nullptr, *n1 = nullptr, *n2 = nullptr;
    if (!m_mesh->N.empty())
    {
        n0 = &m_mesh->N[i0];
        n1 = &m_mesh->N[i1];
        n2 = &m_mesh->N[i2];
    }

    return singleTriangleIntersect(ray, p0, p1, p2, n0, n1, n2, hit, m_material, this);
}

AABB3f Triangle::localBBox() const
{
	// all mesh vertices have already been transformed to world space,
	// so we need to transform back to get the local space bounds
    AABB3f result;
    result.extend(m_xform.inverse() * m_mesh->V[m_face->x()]);
    result.extend(m_xform.inverse() * m_mesh->V[m_face->y()]);
    result.extend(m_xform.inverse() * m_mesh->V[m_face->z()]);
    for (int i = 0; i < 3; ++i) {
        if (result[1][i] - result[0][i] < 1e-4f) {
            result[0][i] -= 5e-5f;
            result[1][i] += 5e-5f;
        }
    }
    return result;
}

AABB3f Triangle::worldBBox() const
{
	// all mesh vertices have already been transformed to world space,
	// so just bound the triangle vertices
    AABB3f result;
    result.extend(m_mesh->V[m_face->x()]);
    result.extend(m_mesh->V[m_face->y()]);
    result.extend(m_mesh->V[m_face->z()]);
    for (int i = 0; i < 3; ++i) {
        if (result[1][i] - result[0][i] < 1e-4f) {
            result[0][i] -= 5e-5f;
            result[1][i] += 5e-5f;
        }
    }
    return result;
}