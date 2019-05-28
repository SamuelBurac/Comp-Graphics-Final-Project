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

#include "transform.h"
#include "ray.h"
#include "parser.h"
#include "rand.h"

/*!
 * @brief This class represents a virtual pinhole camera.
 *
 * The camera is responsible for generating primary rays. It is positioned
 * using a Transform and points along the -z axis of the local coordinate
 * system. It has an image plane positioned a z = -dist with size
 * (width, height).
 *
 * We currently only support pinhole perspective cameras. This class could
 * be made into a virtual base class to support other types of cameras
 * (e.g. an environment camera, or a physically based camera model that simulates
 * the out of focus blur due to a finite aperture and actual lenses).
 */
class Camera
{
public:
    //! Construct a camera from json parameters.
    Camera(const json & j = json())
    {
		m_xform = j.value("transform", m_xform);
	    m_resolution = j.value("resolution", m_resolution);
	    m_focalDistance = j.value("fdist", m_focalDistance);
	    m_apertureRadius = j.value("aperture", m_apertureRadius);
		
		float vfov = 90.f; // Default vfov value. Override this with the value from json
		vfov = deg2rad(j.value("vfov", vfov));
	    m_size.y() = 2*tan(vfov/2) * m_focalDistance;
	    m_size.x() = float(m_resolution.x())/m_resolution.y() * m_size.y();
    }

	//! Return the camera's image resolution
	Vector2i resolution() const {return m_resolution;}


	/*!
	 * @brief	Generate a ray going through image-plane location (u,v).
	 *
	 * (@c u,@c v) range from 0 to m_resolution.x() and m_resolution.y() along the x- and y-axis of the rendered image, respectively
	 *
	 * @param u 	The horizontal position within the image
	 * @param v  	The vertical position within the image
	 * @return 		The @ref Ray3f data structure filled with the appropriate position and direction
	 */
    Ray3f generateRay(float u, float v) const
    {
		u /= m_resolution.x();
		v /= m_resolution.y();

		Vector2f disk = m_apertureRadius*randomInUnitDisk();
		Point3f origin(disk.x(), disk.y(), 0.f);
        return m_xform * Ray3f(origin,
        	                   Vector3f((u - 0.5f) * m_size.x(),
        	                   	        (0.5f - v) * m_size.y(),
        	                   	        -m_focalDistance) - origin);
    }

private:
	//
	// The camera setup looks something like this, where the
	// up vector points out of the screen:
	//
	//         top view                         side view
	//            ^                    up
	//            |                     ^
	//            |                     |             _,-'
	//          width                   |         _,-'   |
	//       +----|----+     +          |     _,-'       | h
	//        \   |   /    d |        e | _,-'           | e
	//         \  |  /     i |        y +'---------------+-i----->
	//          \ | /      s |        e  '-,_   dist     | g
	//           \|/       t |               '-,_        | h
	//            +          +                   '-,_    | t
	//           eye                                 '-,_|
	//


	Transform m_xform = Transform();            //!< Local coordinate system
	Vector2f m_size = Vector2f(1,1);            //!< Physical size of the image plane
	float m_focalDistance = 1.f;                //!< Distance to image plane along local z axis
	Vector2i m_resolution = Vector2i(512,512);  //!< Image resolution
	float m_apertureRadius = 0.f;               //!< The size of the aperture for depth of field
};
