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

#include "quad.h"

Quad::Quad(const Vector2f size,
           const Material * material,
           const Transform & xform)
	: Surface(material, xform), m_size(size/2)
{

}

Quad::Quad(const Scene & scene, const json & j)
    : Surface(scene, j)
{
    // user can specify either a width or a size, size takes priority
    m_size = j.value("size", m_size);

	m_size /= 2.f;
}

bool Quad::intersect(const Ray3f &ray, HitInfo &hit) const
{
    INCREMENT_INTERSECTION_TESTS;

    // compute ray intersection (and ray parameter), continue if not hit
    auto tray = m_xform.inverse() * ray;
    if (tray.d.z() == 0)
        return false;
    auto t = -tray.o.z() / tray.d.z();
    auto p = tray(t);

    if (m_size.x() < p.x() || -m_size.x() > p.x() || m_size.y() < p.y() || -m_size.y() > p.y())
        return false;

    // check if computed param is within ray.mint and ray.maxt
    if (t < tray.mint || t > tray.maxt)
        return false;

	// project hitpoint onto plane to reduce floating-point error
	p.z() = 0;

    Normal3f gn = (m_xform * Normal3f(0,0,1)).normalized();
    // if hit, set intersection record values
    hit = HitInfo(t, m_xform * p, gn, gn,
                         Point2f(p.x() / (2 * m_size.x()) + 0.5f,
                                 p.y() / (2 * m_size.y()) + 0.5f),
                         m_material, this);
    return true;
}


AABB3f Quad::localBBox() const
{
    return AABB3f(-Vector3f(m_size.x(),m_size.y(),0) - Vector3f(1e-4f), Vector3f(m_size.x(),m_size.y(),0) + Vector3f(1e-4f));
}