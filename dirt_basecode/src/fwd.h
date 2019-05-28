/*
    This file is part of Dirt, the Dartmouth introductory ray tracer, used in
    Dartmouth's COSC 77/177 Computer Graphics course.

    Copyright (c) 2017 by Wojciech Jarosz

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

// Forward declarations
template <typename Scalar, int Dimension>  struct Vector;
template <typename Scalar, int Dimension>  struct Point;
template <typename Point, typename Vector> struct Ray;
class BBH;
class Camera;
class Color3f;
class Image3f;
struct HitInfo;
class Light;
class Material;
struct Mesh;
class Progress;
class Quad;
class Scene;
class Sphere;
class Surface;
class SurfaceGroup;
class Timer;
class Transform;
class WavefrontOBJ;
class Texture;


// Dirt's basic Vector and Point data structures
// are oblivious to the underlying data type and dimension.
// The following list of typedefs establishes some convenient aliases
// for specific types.
using Vector1f = Vector<float, 1>;
using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;
using Vector1d = Vector<double, 1>;
using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;
using Vector1i = Vector<int, 1>;
using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;

using Point1f = Point<float, 1>;
using Point2f = Point<float, 2>;
using Point3f = Point<float, 3>;
using Point4f = Point<float, 4>;
using Point1d = Point<double, 1>;
using Point2d = Point<double, 2>;
using Point3d = Point<double, 3>;
using Point4d = Point<double, 4>;
using Point1i = Point<int, 1>;
using Point2i = Point<int, 2>;
using Point3i = Point<int, 3>;
using Point4i = Point<int, 4>;