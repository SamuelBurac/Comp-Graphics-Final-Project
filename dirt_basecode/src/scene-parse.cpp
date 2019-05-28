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
#include "parser.h"
#include "obj.h"
#include "bbh.h"
#include "sphere.h"
#include "quad.h"
#include "texture.h"
#include <iostream>

// local functions
namespace
{

vector<Surface*> parseSurface(const Scene & scene, const json & j)
{
    string type;
    try
    {
        type = j.at("type");
    }
    catch (...)
    {
        throw DirtException("Missing \"type\" on surface specification:\n\t%s",
                            j.dump());
    }

    if (type == "quad")
        return {new Quad(scene, j)};
    else if (type == "sphere")
        return {new Sphere(scene, j)};
    else if (type == "obj")
    {
        auto xform = Transform();
        xform = j.value("transform", xform);
        std::string path = j["filename"];
        Mesh * mesh = new Mesh(loadWavefrontOBJ((scene.sceneDirectory()/path).asString(), xform));

        if (mesh->empty())
            return {};

        vector<Surface*> tris;
        tris.reserve(mesh->F.size());
        for (auto index : range(mesh->F.size()))
            tris.push_back(new Triangle(scene, j, mesh, int(index)));

        return tris;
    }
    else
        throw DirtException("Unknown surface type \"%s\" in specification:\n\t%s",
                            type.c_str(), j.dump());
}


SurfaceGroup* parseAccelerator(const Scene & scene, const json & j)
{
    if (j.count("accelerator"))
    {
        string type;
        try
        {
            type = j["accelerator"].at("type");
        }
        catch (...)
        {
            throw DirtException("Missing \"type\" on accelerator specification");
        }

        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type == "bbh" || type == "bvh")
            return new BBH(scene, j["accelerator"]);
        else
            throw DirtException("Unknown accelerator type %s\n", type.c_str());
    }

    // default to a naive accelerator
    return new SurfaceGroup(scene, j);
}


Material* parseMaterial(const Scene & scene, const json & j)
{
	string type;
	try
	{
		type = j.at("type");
	}
	catch (...)
	{
		warning("Missing \"type\" on material specification:\n\t%s\n Assuming \"lambertian\".\n", j.dump());
        return new Lambertian(j, &scene);
	}

	if (type == "lambertian")
		return new Lambertian(j, &scene);
	else if (type == "metal")
		return new Metal(j, &scene);
    else if (type == "dielectric")
        return new Dielectric(j, &scene);
    else if (type == "diffuse light")
        return new DiffuseLight(j, &scene);
	else
		throw DirtException("Unknown material type \"%s\" in specification:\n\t%s",
		                    type.c_str(), j.dump());
}

} // namespace



const Material * Scene::findOrCreateMaterial(const json & j) const
{
    if (!j.count("material"))
        return Material::defaultMaterial();

    if (j["material"].is_string())
    {
        // find a pre-declared material
        auto mat = m_materials.find(j["material"]);
        if (mat != m_materials.end())
            return mat->second;
        else
            return Material::defaultMaterial();
    }
    else if (j["material"].is_object())
    {
	    // create a new material
	    return parseMaterial(*this, j["material"]);
    }
    else
        return Material::defaultMaterial();
}

void Scene::parseFromJSON(const json & j)
{
    if (j.empty())
    {
        m_surfaces = parseAccelerator(*this, j);
        m_surfaces->build();
        return;
    }
    message("parsing...\n");

    // create the scene-wide acceleration structure
    m_surfaces = parseAccelerator(*this, j);

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        if (it.key() == "camera")
        {
            if (m_camera)
                throw DirtException("There can only be one camera per scene!");
            m_camera = new Camera(it.value());
        }
        else if (it.key() == "materials")
        {
            for (auto & m : it.value())
            {
                auto material = parseMaterial(*this, m);
                string name;
                try
                {
                    name = m.at("name");
                }
                catch (...)
                {
                    throw DirtException("Missing \"name\" on material specification");
                }
                m_materials[name] = material;
            }
        }
        else if (it.key() == "surfaces")
        {
            for (auto & s : it.value())
            {
                auto surfaces = parseSurface(*this, s);
                for (auto surface : surfaces)
                {
	                m_surfaces->addChild(surface);
                }
            }
        }
        else if (it.key() == "image_samples")
        {
            m_imageSamples = it.value();
        }
        else if (it.key() == "background")
        {
            m_background = it.value();
        }
        else if (it.key() == "accelerator")
        {
            // already handled above
        }
        else
            throw DirtException("unsupported keyword \"%s\"!", it.key());
    }

    m_surfaces->build();
    message("done parsing scene.\n");
}

Scene* create_test_scene_sphere()
{
    std::string test = R"(
    {
        "camera":
        {
            "transform": { "o": [0,0,2.5] },
            "resolution": [ 512, 512 ]
        },
        "surfaces": [
            {
                "type": "sphere",
                "material": { "type": "lambertian", "albedo": [0.6,0.6,0.6] }
            }
        ],
        "image_samples": 10,
        "background": [0.1, 0.1, 0.1]
    }
    )";
    return new Scene(json::parse(test));
}

Scene* create_test_scene_sphereplane()
{
    std::string test = R"(
    {
        "camera":
        {
            "transform": { "o": [0,0,4] },
            "resolution": [ 512, 512 ]
        },
        "surfaces": [
            {
                "type": "sphere",
                "radius": 1,
                "material": { "type": "lambertian", "albedo": [0.6,0.4,0.4] }
            },
            {
                "type": "quad",
                "transform": { "o": [0,-1,0], "x": [1,0,0], "y": [0,0,-1], "z": [0,1,0] },
                "size": [ 100, 100 ],
                "material": { "type": "lambertian", "albedo": [1,1,1] }
            }
        ],
        "image_samples": 100,
        "background": [1, 1, 1]
    }
    )";
    return new Scene(json::parse(test));
}

Scene* create_test_scene_steinbach_screw()
{
    json jobj;

    // Compose the camera
    jobj["camera"] =
	    {
		    {"transform", {{"from", {5.0, 15.0, -25.0}},
		                   {"to",   {0.0,  0.0,   0.0}},
		                   {"up",   {0.0,  1.0,   0.0}}}},
		    {"vfov", 22},
            {"resolution", { 512, 512 }}
	    };

    // compose the image properties
    jobj["image_samples"] = 10;
    jobj["background"] = {2, 2, 2};

//    jobj["accelerator"] =
//        {
//            {"type", "bbh"},
//            {"maxPrimsInNode", 3},
//            {"splitMethod", "sah"}
//        };

    Vector3f objectCenter(0.0f, 0.0f, 0.0f);
    float radius = 0.5f;
    int numI = 40;
    int numJ = 40;
    for (auto i : range(numI))
    {
        for (auto j : range(numJ))
        {
            float s = (i + 0.5f) / numI;
            float t = (j + 0.5f) / numJ;
            float u = s * (8) - 4.0f;
            float v = t * (6.25f);
            Vector3f center(-u * cos(v), v * cos(u) * 0.75f, u * sin(v));
            Color3f kd = 0.35f*lerp(lerp(Color3f(0.6f, 0.0f, 0.0f), Color3f(0.0f, 0.6f, 0.0f), t),
                                lerp(Color3f(0.0f, 0.0f, 0.6f), Color3f(0.0f, 0.0f, 0.0f), t), s);

	        jobj["surfaces"] +=
	            {
	                {"type", "sphere"},
	                {"radius", radius},
	                {"transform", {{ "o", (objectCenter + center).eval() },
	                               { "x", { 1.0, 0.0, 0.0 } },
	                               { "y", { 0.0, 1.0, 0.0 } },
	                               { "z", { 0.0, 0.0, 1.0 } }}},
	                {"material", {{"type", "lambertian"},
                                  {"albedo", kd}}}
				};
        }
    }

    jobj["surfaces"] +=
        {
            {"type", "quad"},
            {"size", {100, 100}},
            {"transform", {{ "o", { 0.0,-5.0, 0.0} },
                           { "x", { 1.0, 0.0, 0.0 } },
                           { "y", { 0.0, 0.0,-1.0 } },
                           { "z", { 0.0, 1.0, 0.0 } }}},
            {"material", {{"type", "lambertian"}, {"albedo", {0.2,0.2,0.2}}}}
        };

    return new Scene(jobj);
}

Scene* create_test_scene_shirley()
{
    json jobj;

    // Compose the camera
    jobj["camera"] =
        {
            {"transform", {{"from", {13,2,3}},
                           {"to",   {0,0,0}},
                           {"up",   {0,1,0}}}},
            {"vfov", 20},
            {"fdist", 10},
            {"aperture", 0.1},
            {"resolution", { 600, 400 }}
        };

    // compose the image properties
    jobj["image_samples"] = 100;

    jobj["background"] = {1,1,1};

//    jobj["accelerator"] =
//        {
//            {"type", "bbh"},
//            {"maxPrimsInNode", 3},
//            {"splitMethod", "sah"}
//        };


    // ground plane
    jobj["surfaces"] +=
        {
            {"type", "quad"},
            {"size", {100, 100}},
            {"transform", {{ "o", { 0.0, 0.0, 0.0} },
                           { "x", { 1.0, 0.0, 0.0 } },
                           { "y", { 0.0, 0.0,-1.0 } },
                           { "z", { 0.0, 1.0, 0.0 } }}},
            {"material", {{"type", "lambertian"},
                          {"albedo", {0.5, 0.5, 0.5} }}}
        };


    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float choose_mat = randf();
            Vector3f center(a+0.9f*randf(),0.2f,b+0.9f*randf());
            if ((center-Vector3f(4.0f,0.2f,0.0f)).norm() > 0.9f)
            {
                json sphere =
                {
                    {"type", "sphere"},
                    {"radius", 0.2f},
                    {"transform", {{ "translate", center.eval() }}}
                };

                if (choose_mat < 0.8)
                {  // diffuse
                    Color3f albedo(randf()*randf(), randf()*randf(), randf()*randf());
                    sphere["material"] =
                        {
                            {"type", "lambertian"},
                            {"albedo", albedo.eval()}
                        };
                }
                else if (choose_mat < 0.95)
                { // metal
                    Color3f albedo(0.5f*(1 + randf()), 0.5f*(1.0f + randf()), 0.5f*(1.0f + randf()));
                    float rough = 0.5f*randf();
                    sphere["material"] =
                        {
                            {"type", "metal"},
                            {"albedo", albedo.eval()},
                            {"roughness", rough}
                        };
                }
                else
                {  // glass
                    sphere["material"] =
                        {
                            {"type", "dielectric"},
                            {"ior", 1.5}
                        };
                }

                jobj["surfaces"] += sphere;
            }
        }
    }


    jobj["surfaces"] +=
        {
            {"type", "sphere"},
            {"radius", 1.f},
            {"transform", {{ "translate", {0,1,0} }}},
            {"material", {{"type", "dielectric"},
                          {"ior", 1.5}}}
        };
    jobj["surfaces"] +=
        {
            {"type", "sphere"},
            {"radius", 1.f},
            {"transform", {{ "translate", {-4,1,0} }}},
            {"material", {{"type", "lambertian"},
                          {"albedo", {0.4,0.2,0.1} }}}
        };
    jobj["surfaces"] +=
        {
            {"type", "sphere"},
            {"radius", 1.f},
            {"transform", {{ "translate", {4,1,0} }}},
            {"material", {{"type", "metal"},
                          {"albedo", {0.7, 0.6, 0.5} },
                          {"roughness", 0.0 }}}
        };

    return new Scene(jobj);
}


Scene* createTestScene(int sceneNumber)
{
    switch (sceneNumber)
    {
        case 0: return create_test_scene_sphere();
        case 1: return create_test_scene_sphereplane();
        case 2: return create_test_scene_steinbach_screw();
        case 3: return create_test_scene_shirley();
    }
    
    error("unknown test scene type %d\n", sceneNumber);
    return nullptr;
}
