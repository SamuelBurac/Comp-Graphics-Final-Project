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
#include "material.h"
#include "transform.h"
#include "parser.h"


/*!
 * @brief   Contains information about a ray-surface intersection hit point.
 *
 * Used by surface intersection routines in order to return more than just a
 * single value. Includes the position, traveled ray distance, uv coordinates,
 * the geometric and interpolated shading normals, and a pointer to the
 * intersected surface and underlying material.
 */
struct HitInfo
{
	float t;                            //!< Ray parameter for the hit
	Point3f p;                          //!< Hit position
	Normal3f gn;                        //!< Geometric normal
	Normal3f sn;                        //!< Interpolated shading normal
	Point2f uv;                         //!< UV texture coordinates
	const Material* mat = nullptr;      //!< Material at the hit point
	const Surface * surface = nullptr;  //!< Surface at the hit point

	//! Default constructor that leaves all members uninitialized
	HitInfo()
	{

	}

	//! Parameter constructor that initializes all data members
	HitInfo(float t,
            const Point3f &p,
            const Normal3f &gn,
            const Normal3f &sn,
            const Point2f &uv,
            const Material * m = nullptr,
            const Surface * s = nullptr) :
		t(t), p(p), gn(gn), sn(sn), uv(uv), mat(m), surface(s)
	{

	}
};


/*!
 * @brief   Superclass for all surfaces.
 *
 * Surfaces represent the geometry of the scene. A @c Surface could be an
 * individual primitive like a @ref Sphere, or it could be composed of
 * many smaller primitives, e.g. the triangles composing a @ref Mesh.
 * Each surface currently stores a transformation matrix which positions/orients
 * the surface in the scene and pointer to a single @ref Material which
 * specifies the light reflectance properties.
 */
class Surface
{
public:
	Surface(const Material * material = Material::defaultMaterial(),
			const Transform & xform = Transform()) :
		m_xform(xform), m_material(material) {}

    Surface(const Scene & scene, const json & j = json::object());
    virtual ~Surface() {}

    //! Return the surface's local-space AABB.
    virtual AABB3f localBBox() const = 0;

    //! Return the surface's world-space AABB.
    /*!
        By default just returns the local bounding box transformed to world 
        space by the Surface's transformation.
    */
    virtual AABB3f worldBBox() const;

    //! Add a child surface (if this is an aggregate).
    /*!
        This function will become useful once we create groups of objects. The base class implementation just throws an error.

        This function should only be used before @ref build() is called.
    */
    virtual void addChild(Surface *surface)
    {
        throw DirtException("This surface does not support children.");
    }

    //! Perform any necessary precomputation before ray tracing.
    /*!
        If a surface requires some precomputation (e.g. building an 
        acceleration structure) overload this function. This will be called 
        after the last child has been added and before any call to intersect().

        The base class implementation just does nothing.
    */
    virtual void build() {};

    //! Ray-Surface intersection test
    virtual bool intersect(const Ray3f &ray, HitInfo &hit) const = 0;

protected:
    Transform m_xform = Transform();        //!< Transformation
    const Material* m_material;             //!< Material
};
