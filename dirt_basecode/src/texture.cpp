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

#include "texture.h"
#include "scene.h"


Texture* Texture::parse(const string& name, const json& j, Color3f defaultColor, const Scene* scene) {

	json texture_desc;
	try
	{
		texture_desc = j.at(name);
	}
	catch (...)
	{
		return new ConstantTexture(
			{ 
				{ "value", defaultColor }
			}, 
			scene);
	}


	if (texture_desc.is_array()) {
		return new ConstantTexture(
			{
				{ "value", texture_desc.get<Color3f>() }
			},
			scene);
	} 
	else if (texture_desc.is_number()) {
		return new ConstantTexture(
			{
				{ "value", Color3f(texture_desc.get<float>()) }
			},
			scene);
	}
	else if (texture_desc["type"] == "constant") {
		return new ConstantTexture(texture_desc, scene);
	}
	else if (texture_desc["type"] == "image") {
		return new ImageTexture(texture_desc, scene);
	}
	else {
		throw DirtException("Failed to create texture from blob: %s", texture_desc.dump());
	}
}