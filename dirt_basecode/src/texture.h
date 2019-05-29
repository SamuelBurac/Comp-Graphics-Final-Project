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
#include "parser.h"
#include "color.h"
#include "image.h"
#include "surface.h"
#include "scene.h"

//! A base class used to represent a spatially varying color.
class Texture
{
public:
	virtual ~Texture() = default;

	/*!
	 * @brief Compute the color at a surface hit point.
	 *
	 * The base Texture is completely black, so just return Color3f(0).
	 *
	 * @param[in] 	hit			the ray's intersection with the surface
	 * @return					the sampled color
	 */
	virtual Color3f sample(const HitInfo& hit) const
	{
		return Color3f(0);
	}


	static Texture* parse(const string& name, const json& j = json::object(), Color3f defaultColor = Color3f(), const Scene* scene = nullptr);
};

class ConstantTexture : public Texture {
public:

	ConstantTexture(const json& j = json::object(), const Scene* scene = nullptr) {
		value = j["value"];
	}

	virtual Color3f sample(const HitInfo& hit) const override
	{
		return value;
	}

	Color3f value;
};

class ImageTexture : public Texture {
public:
	ImageTexture(const json& j = json::object(), const Scene* scene = nullptr) {

		std::string path = j["image"];
		
		std::string scene_path = path; 
		if (scene != nullptr) {
			scene_path = (scene->sceneDirectory() / path).asString();
		}

		image.load(scene_path);
	}

	virtual Color3f sample(const HitInfo& hit) const override
	{
		float x = hit.uv.x() * image.width();
		float y = (1.0 - hit.uv.y()) * image.height();

		// Point sampling
		return image((int)x, (int)y);
	}

	Image3f image;
};