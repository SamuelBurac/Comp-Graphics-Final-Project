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

#include "sphere.h"



Sphere::Sphere(float radius,
               const Material * material,
               const Transform & xform)
    : Surface(material, xform), m_radius(radius)
{

}

Sphere::Sphere(const Scene & scene, const json & j)
    : Surface(scene, j)
{
	m_radius = j.value("radius", m_radius);
}

AABB3f Sphere::localBBox() const
{
    return AABB3f(-m_radius*Vector3f::Ones(), m_radius*Vector3f::Ones());
}


bool Sphere::intersect(const Ray3f &ray, HitInfo &hit) const
{
    INCREMENT_INTERSECTION_TESTS;

	//////////////////////////////////////////////////////////////////////////
	////This is your implementation to calculate t
	////terminate early by returning false
	auto tray = m_xform.inverse() * ray;	////transform ray to local space
	Vector3f d=tray.d.normalized();			////ray direction in local space
	Vector3f o=tray.o;						////ray origin in local space
	float r=m_radius;						////sphere radius
	Vector3f c=Vector3f::Zero();			////sphere center, origin in local space
	float t=0.f;


	//////////////////////////////////////////////////////////////////////////

    // check if computed param is within ray.mint and ray.maxt
    t/=tray.d.norm();
	if (t < tray.mint || t > tray.maxt)
        return false;

    auto p = tray(t);
    p *= m_radius / p.norm();

    Normal3f gn = (m_xform * Normal3f(p)).normalized();
    // if hit, set intersection record values
    hit = HitInfo(t, m_xform * p, gn, gn, Point2f(0,0), m_material, this);
    return true;
	//////////////////////////////////////////////////////////////////////////
}
