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

    This file is based on the TRay class from Nori:

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include "vector.h"

/*!
 * @brief   Simple ray segment data structure.
 *
 * Along with the ray origin and direction, this data structure additionally
 * stores the segment interval [mint, maxt] (whose entries may include positive/
 * negative infinity).
 */
struct Ray3f
{
    Point3f o;         //!< The origin of the ray
    Vector3f d;        //!< The direction of the ray
    float mint;        //!< Minimum distance along the ray segment
    float maxt;        //!< Maximum distance along the ray segment

    //! Construct a new ray
    Ray3f() : mint(Epsilon),
        maxt(std::numeric_limits<float>::infinity()) { }

    //! Construct a new ray
    Ray3f(const Point3f &o, const Vector3f &d) : o(o), d(d),
            mint(Epsilon), maxt(std::numeric_limits<float>::infinity()) { }

    //! Construct a new ray
    Ray3f(const Point3f &o, const Vector3f &d,
        float mint, float maxt) : o(o), d(d), mint(mint), maxt(maxt) { }

    //! Copy constructor
    Ray3f(const Ray3f &ray)
     : o(ray.o), d(ray.d), mint(ray.mint), maxt(ray.maxt) { }

    //! Copy a ray, but change the covered segment of the copy
    Ray3f(const Ray3f &ray, float mint, float maxt)
     : o(ray.o), d(ray.d), mint(mint), maxt(maxt) { }

    //! Return the position of a point along the ray
    Point3f operator() (float t) const { return o + t * d; }
};
