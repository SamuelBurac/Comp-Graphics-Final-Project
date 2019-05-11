# DIRT - Dartmouth Introductory Ray Tracer

Dirt is a simple ray tracer written in C++. It provides basic functionalities
that are required to complete the assignments for the Computer Graphics course (CS77/177) taught at Dartmouth College.

### High Level Overview
***********************

The Dirt repository contains source code that are a part of the actual ray tracer as well as other external libraries that dirt depends upon.
The external libraries that dirt depends upon are

1. json for modern c++ - for handling json input file formats of scenes
2. stb image library - for loading and storing images
3. tiny format - pretty formatting and printing of strings

The major components of the dirt can be split into the following.

* Utility
    + color.h - contains code that handles and manipulates RGB color data.
    + common.h - contains utility code used throughout dirt. 
    + image.h - contains code that handles and manipulates image data. 
    + obj.h - contains code to load geometry data from OBJ files.
    + parser.h - contains code that parses JSON scene files. 
    + progress.h - contains code that pretty prints the rendering progress.
* Math
    + transform.h - contains code that implements 3-D transformations.
    + vector.h - contains code that represents and manipulates 3-D vectors.
* Raytracing
    + accelerator.h - contains base class for all accelerators that speed up raytracing.
    + camera.h - contains the camera class that represents the virtual camera placed inside a scene.
    + light.h - contains the light class that represents an illumination source within the scene.
    + material.h - contains the material class that determines the shaded color of an object.
    + ray.h - contains the ray class that is the basic unit of the ray tracer.
    + surface.h - contains the base surface class that represents any geometry(simple or composite) within the scene.
    + scene.h - contains the scene class that stores all geometry, lights and camera.

### Building Instructions
*************************

The instructions for building dirt are provided in the course website.

### License Information
***********************

Check the LICENSE file for information about the license under which dirt is distributed.
