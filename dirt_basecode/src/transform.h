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

    This file is based on the Transform class from Nori:

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include "common.h"
#include "vector.h"
#include "ray.h"


//! Homogeneous coordinate transformation
/*!
    This class stores a general homogeneous coordinate transformation, such as
    rotation, translation, uniform or non-uniform scaling, and perspective
    transformations. The inverse of this transformation is also recorded
    here, since it is required when transforming normal vectors.
*/
class Transform
{
public:
    //! Create the identity transform
    Transform() :
        m_transform(Eigen::Matrix4f::Identity()),
        m_inverse(Eigen::Matrix4f::Identity()) {}

    //! Create a new transform instance for the given matrix
    Transform(const Eigen::Matrix4f &m)
        : m_transform(m), m_inverse(m.inverse()) { }

    //! Create a new transform instance for the given matrix and its inverse
    Transform(const Eigen::Matrix4f &trafo, const Eigen::Matrix4f &inv)
        : m_transform(trafo), m_inverse(inv) {}

    //! Return the underlying matrix
    const Eigen::Matrix4f & getMatrix() const
    {
        return m_transform;
    }

    //! Return the inverse of the underlying matrix
    const Eigen::Matrix4f & getInverseMatrix() const
    {
        return m_inverse;
    }

    //! Return the inverse transformation
    Transform inverse() const
    {
        return Transform(m_inverse, m_transform);
    }

    //! Concatenate with another transform
    Transform operator*(const Transform &t) const
    {
        return Transform(m_transform * t.m_transform, t.m_inverse * m_inverse);
    }

    //! Apply the homogeneous transformation to a 3D vector
    Vector3f operator*(const Vector3f &v) const
    {
        return m_transform.topLeftCorner<3,3>() * v;
    }

    //! Apply the homogeneous transformation to a 3D normal
    Normal3f operator*(const Normal3f &n) const
    {
		Normal3f m = m_inverse.topLeftCorner<3, 3>().transpose() * n;
        return m/m.norm();
    }

    //! Transform a point by an arbitrary matrix in homogeneous coordinates
    Point3f operator*(const Point3f &p) const
    {
		Vector4f result = m_transform * Vector4f(p[0], p[1], p[2], 1.0f);
        return result.head<3>() / result.w();
    }

    //! Apply the homogeneous transformation to a ray
    Ray3f operator*(const Ray3f &r) const
    {
		return Ray3f(operator*(r.o),
                operator*(r.d),
                r.mint, r.maxt);
    }

    //! Transform the Box and return the resulting bounding box
    AABB3f operator*(const AABB3f & box) const
    {
        // a transformed empty box is still empty
        if (box.isEmpty())
            return box;

        // Just in case this is a projection matrix, do things the naive way.
        Point3f pts[8];

        // Set up the eight points at the corners of the extent
        pts[0].x() = pts[1].x() = pts[2].x() = pts[3].x() = box.min().x();
        pts[4].x() = pts[5].x() = pts[6].x() = pts[7].x() = box.max().x();

        pts[0].y() = pts[1].y() = pts[4].y() = pts[5].y() = box.min().y();
        pts[2].y() = pts[3].y() = pts[6].y() = pts[7].y() = box.max().y();

        pts[0].z() = pts[2].z() = pts[4].z() = pts[6].z() = box.min().z();
        pts[1].z() = pts[3].z() = pts[5].z() = pts[7].z() = box.max().z();

        AABB3f newBox;
        for (int i = 0; i < 8; i++)
            newBox.extend(operator*(pts[i]));

        return newBox;
    }

    static Transform translate(Point3f p)
    {
        Matrix4f tform;
        tform << 1.0f, 0.0f, 0.0f, p.x(),
                 0.0f, 1.0f, 0.0f, p.y(),
                 0.0f, 0.0f, 1.0f, p.z(),
                 0.0f, 0.0f, 0.0f, 1.0f;
        return Transform(tform);
    }

    static Transform axisOffset(Vector3f x, Vector3f y, Vector3f z, Point3f o)
    {
        Matrix4f tform;
        tform << x[0], y[0], z[0], o[0],
                 x[1], y[1], z[1], o[1],
                 x[2], y[2], z[2], o[2],
                 0.0f, 0.0f, 0.0f, 1.0f;
        return Transform(tform);
    }

private:
    Eigen::Matrix4f m_transform;
    Eigen::Matrix4f m_inverse;
};
