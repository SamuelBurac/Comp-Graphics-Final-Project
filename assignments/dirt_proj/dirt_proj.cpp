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

#include "common.h"
#include "image.h"
#include "ray.h"
#include "camera.h"
#include "sphere.h"
#include "scene.h"


int main(int argc, char **argv)
{
	std::string scene_file = argc > 1 ? argv[1] : /*"scenes/01_raytrace/06_cornell-box.json"; /*/ "scenes/03_raytrace_texture/03_bunny.json";
	std::string output_file = argc > 2 ? argv[2] : "output.png";
	Scene scene(scene_file);
	Image3f img = scene.raytrace();
	img.save(output_file);
    return 0;
}
