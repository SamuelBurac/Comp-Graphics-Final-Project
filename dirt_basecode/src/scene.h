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

#include "common.h"
#include "image.h"
#include "camera.h"
#include "ray.h"
#include "material.h"
#include "surfacegroup.h"
#include "path.h"


/*!
 * @brief Main scene data structure.
 *
 * This class holds information on scene objects and is responsible for
 * coordinating rendering jobs.
 */
class Scene
{
public:
    //! Construct a new scene from a json object
    Scene(const json & j = json()) {parseFromJSON(j);}
    //! Read a scene from a json file
    Scene(const string & filename) : m_path(filename) { parseFromJSON(loadJSON(filename)); }

    //! Release all memory
    virtual ~Scene();

    //! Parser a scene from a json object
    void parseFromJSON(const json & j);

    /*!
     * @brief Find/create a material.
     *
     * Return a Material pointer by parsing the "material" specification.
     * If @c j is a string "material": "material-name", then try to find
     * a material with name "material-name" in the pre-declared map of scene
     * materials. If @c j is a json object "material": {}, then create a new
     * material with the specified parameters.
     */
    const Material * findOrCreateMaterial(const json & j) const;

    /*!
     * @brief Intersect a ray against all triangles stored in the scene and return detailed intersection information
     * @param ray
     *      A 3-dimensional ray data structure with minimum/maximum
     *      extent information
     * @param its
     *      A detailed intersection record, which will be filled by the
     *      intersection query
     * @return  @c true if an intersection was found
     */
    bool intersect(const Ray3f & ray, HitInfo & its) const
    {
        return m_surfaces->intersect(ray, its);
    }

    /*!
     * @brief Sample the incident radiance along a ray
     * @param ray
     *      The ray in question
     * @return
     *      An estimate of the radiance in this direction
     */
    Color3f recursiveColor(const Ray3f &ray, int depth) const;

    //! Generate the entire image by ray tracing.
    Image3f raytrace() const;

    Path sceneDirectory() const { return m_path.parent(); }

private:
    Camera * m_camera = nullptr;
    std::map<std::string, const Material *> m_materials;
    SurfaceGroup * m_surfaces = nullptr;
    Color3f m_background = Color3f::Ones()*0.2f;

    Path m_path;

    int m_imageSamples = 1;                      //!< samples per pixels in each direction
};

// create test scenes that do not need to be loaded from a file
Scene* createTestScene(int sceneNumber);
