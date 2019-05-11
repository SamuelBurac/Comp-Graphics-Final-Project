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

#include "scene.h"
#include "progress.h"

uint64_t intersection_tests = 0;
uint64_t rays_traced = 0;

Scene::~Scene()
{
    delete m_surfaces;
    delete m_camera;
    for (auto m: m_materials)
        delete m.second;
    m_materials.clear();
}

// compute the color corresponding to a ray by raytracing
Color3f Scene::recursiveColor(const Ray3f &ray, int depth) const
{
    put_your_code_here("Assignment 1: Insert your raytrace_ray() code here");
    return Color3f(0.0f, 0.0f, 0.0f);

    // TODO: Recursively raytrace the scene, similar to the code you wrote in 01_dirt_tutorial
    //       Different to before, you should also take into account surfaces that are self-emitting
    // Pseudo-code:
    //
	// if scene.intersect:
    //      get emitted color (hint: you can use hit.mat->emitted)
	// 		if depth < MaxDepth and hit_material.scatter(....) is successful:
	//			recursive_color = call this function recursively with the scattered ray and increased depth
	//          return emitted color + attenuation * recursive_color
	//		else
	//			return emitted color;
	// else:
	// 		return background color (hint: look at m_background)
}

// raytrace an image
Image3f Scene::raytrace() const
{
    // allocate an image of the proper size
    auto image = Image3f(m_camera->resolution().x(), m_camera->resolution().y());

    put_your_code_here("Assignment 1: insert your raytrace() code here");

    // TODO: Render the image, similar to the tutorial
    // Pseudo-code:
    //
        // foreach image row (go over image height)
            // foreach pixel in the row (go over image width)
                // init accumulated color to zero
                // repeat m_imageSamples times:
                    // compute a random point within the pixel (you can just add a random number between 0 and 1
                    //                                          to the pixel coordinate. You can use randf() for this)
                    // compute camera ray
                    // accumulate color raytraced with the ray
                // divide color by the number of pixel samples

    // Hint: you can create a Progress object (progress.h) to provide a 
    // progress bar during rendering.


	// return the ray-traced image
    return image;
}
