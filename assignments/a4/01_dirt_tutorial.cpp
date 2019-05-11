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
#include "surfacegroup.h"

template <typename Vec>
float maxAbsVecError(const Vec & a, const Vec & b)
{
	return (a - b).cwiseAbs().maxCoeff();
}

Color3f vec2color(const Vector3f & dir)
{
    return 0.5f * (Color3f(dir.array()) + 1.f);
}

Color3f rayToColor(const Ray3f & r)
{
    return vec2color(r.d.normalized());
}

Color3f intersectionToColor(const Ray3f & r, const Sphere & sphere)
{
    HitInfo hit;
    if (sphere.intersect(r, hit))
        return vec2color(hit.sn.normalized());
    else
        return vec2color(r.d.normalized());
}

//////////////////////////////////////////////////////////////////////////
////Assignment 4 - Part 1: Generate rays by hand
// Generate rays by hand
void testManualCameraImage()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Assignment 4 - Part 1: Generating rays by hand \n");
	message("--------------------------------------------------------\n");

	// Setup the output image
	Image3f rayImage(200, 100);

	const Vector3f cameraOrigin(0.f, 0.f, 0.f);
	const float imagePlaneWidth = 4.f;
	const float imagePlaneHeight = 2.f;

	// loop over all pixels and generate a ray
	for (auto y : range(rayImage.height()))
	{
		for (auto x : range(rayImage.width()))
		{
			// TODO: Fill in rayOrigin so that the ray starts at the cameraOrigin,
			// and fill in rayDirection so that
			// 1) the x component of the direction varies from -imagePlaneWidth/2 for the left-most pixel to +imagePlaneWidth/2 for the right-most pixel
			// 2) the y component of the direction varies from +imagePlaneHeight/2 for the top-most pixel to -imagePlaneHeight/2 for the bottom-most pixel
			// 3) the z component is -1

			//////////////////////////////////////////////////////////////////////////
			////Here is your implementation to modify rayOrigin and rayDirection
			Vector3f rayOrigin;
			Vector3f rayDirection;
			auto ray = Ray3f(rayOrigin, rayDirection);

			//////////////////////////////////////////////////////////////////////////

			// Generate a visual color for the ray so we can debug our ray directions
			rayImage(x, y) = rayToColor(ray);
		}
	}

	message("Saving ray image to 01_manual_ray_image.png....\n");
	rayImage.save("part_1_manual_ray_image.png");
}

//////////////////////////////////////////////////////////////////////////
////Assignment 4 - Part 2: implement ray-sphere intersection
void testRaySphereIntersection()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Assignment 4 - Part 2: Ray-Sphere intersection \n");
	message("--------------------------------------------------------\n");

	// TODO: Go to dart_basecode/src/sphere.cpp and implement Sphere::intersect

	// Let's check if your implementation was correct:
	Sphere testSphere(1.f);

	message("Testing untransformed sphere intersection\n");
	Ray3f testRay(Point3f(-0.25f, 0.5f, 4.0f), Vector3f(0.0f, 0.0f, -1.0f));
	HitInfo its;
	if (testSphere.intersect(testRay, its))
	{
		float correctT = 3.170844f;
		Point3f correctP(-0.25f, 0.5f, 0.829156f);
		Normal3f correctN(-0.25f, 0.5f, 0.829156f);

		message("Hit sphere! Distance is: %f, and it should be: %f.\n", its.t, correctT);
		message("Intersection point is:\n%s,\nand it should be:\n%s.\n", its.p, correctP);
		message("Intersection normal is:\n%s,\nand it should be:\n%s.\n", its.sn, correctN);

		float sphereError = std::max(std::max(maxAbsVecError(correctP, its.p),
		                                      maxAbsVecError(correctN, its.sn)),
		                             std::abs(correctT - its.t));

		if (sphereError > 1e-5f)
			error("Result incorrect!\n");
		else
			message("Result correct!\n");
	}
	else
		error("Sphere intersection incorrect! Should hit sphere\n");

	// Now, let's check if you implemented sphere transforms correctly!
	auto transform = Transform::axisOffset(
		Vector3f(2.0,  0.0,   0.0), // x-axis
		Vector3f(0.0,  1.0,   0.0), // y-axis
		Vector3f(0.0,  0.0,   0.5), // z-axis
		Point3f (0.0, 0.25,   5.0)  // translation
	);
	Sphere transformedSphere(1.0f, nullptr, transform);
	testRay = Ray3f(Point3f(1.0f, 0.5f, 8.0f), Vector3f(0.0f, 0.0f, -1.0f));
	its = HitInfo();

	bool result=false;
	message("Testing transformed sphere intersection\n");
	if (transformedSphere.intersect(testRay, its))
	{
		float correctT = 2.585422;
		Point3f correctP(1.0f, 0.5f, 5.41458f);
		Normal3f correctN(0.147442f, 0.147442f, 0.978019f);

		message("Hit sphere! Distance is: %f, and it should be: %f.\n", its.t, correctT);
		message("Intersection point is:\n%s,\nand it should be:\n%s.\n", its.p, correctP);
		message("Intersection normal is:\n%s,\nand it should be:\n%s.\n", its.sn, correctN);

		float sphereError = std::max(std::max(maxAbsVecError(correctP, its.p),
		                                      maxAbsVecError(correctN, its.sn)),
		                             std::abs(correctT - its.t));

		if (sphereError > 1e-5f)
			error("Result incorrect!\n");
		else{
			result=true;
			message("Result correct!\n");
		}
	}
	else
		error("Transformed sphere intersection incorrect! Should hit sphere\n");

	if(!result)return;
	//////////////////////////////////////////////////////////////////////////
	////ray trace a sphere
	// Setup the output image
	Image3f rayImage(200, 100);
	// Set up a camera with some reasonable parameters
	Camera camera({
		              {"vfov", 90.f},
		              {"resolution", Vector2i(rayImage.width(), rayImage.height())},
		              {"fdist", 1.f},
		              {"transform", {{"from", {5.0, 15.0, -25.0}},
		                             {"to",   {0.0,  0.0,   0.0}},
		                             {"up",   {0.0,  1.0,   0.0}}} },
	              });

	Sphere sphere(10.f);

	// Generate a ray for each pixel in the ray image
	for (auto y : range(rayImage.height()))
	{
		for (auto x : range(rayImage.width()))
		{
			// TODO: Look in camera.h and implement camera.generateRay

			// Make sure to take the camera transform into account!

			// We add 0.5 to the pixel coordinate to center the ray within the pixel
			auto ray = camera.generateRay(x + 0.5f, y + 0.5f);

			// If we hit the sphere, output the sphere normal; otherwise, convert the ray direction into a color
			// so we can have some visual debugging
			rayImage(x, y) = intersectionToColor(ray, sphere);
		}
	}

	message("Saving ray image to part_2_sphere_image.png....\n");
	rayImage.save("part_2_sphere_image.png");
}

//////////////////////////////////////////////////////////////////////////
////Assignment 4 - Part 3: recursive ray tracing
Color3f recursiveColor(const Ray3f &ray, const SurfaceGroup &scene, int depth)
{
	const int MaxDepth = 64;

	// TODO: Implement this function
	// Pseudo-code:
	//
	// if scene.intersect:
	// 		if depth < MaxDepth and hit_material.scatter(....) is successful:
	//			recursive_color = call this function recursively with the scattered ray and increased depth
	//          return attenuation * recursive_color
	//		else
	//			return black;
	// else:
	// 		return white

	//return Color3f(0.0f);
	
	//////////////////////////////////////////////////////////////////////////
	////Here is your implementation
	HitInfo hit;
    if (scene.intersect(ray, hit)) {
		// recursively trace ray and return color
    
	} else {
		// We hit the background. Let's return white color to simulate a white sky
    
	}

	return Color3f(0.f,0.f,0.f);	////comment out this line after your implementation
	//////////////////////////////////////////////////////////////////////////
}

// Now that we can scatter off of surfaces, let's try this out and render a scene with different materials
void testRecursiveRaytracing()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Assignment 4 - Part 3: Recursive Ray Tracing\n");
	message("--------------------------------------------------------\n");

	// Setup the output image
	Image3f rayImage(300, 150);

	// We want to average over several rays to get a more pleasing result
	const int NumSamples = 64;

	// Set up a camera with some reasonable parameters
	Camera camera({
		              {"vfov", 45.f},
		              {"resolution", Vector2i(rayImage.width(), rayImage.height())},
		              {"fdist", 1.f},
		              {"transform", {{"from", {1.9, 0.8, -3.5}},
		                             {"to",   {1.9, 0.8,  0.0}},
		                             {"up",   {0.0, 1.0,  0.0}}} },
	              });

	Lambertian groundMaterial(json{{"albedo", Vector3f(0.5f, 0.5f, 0.5f)}});
	Lambertian matteMaterial(json{{"albedo", Vector3f(1.0f, 0.25f, 0.25f)}});
	Metal shinyMaterial(json{{"albedo", Vector3f(1.0f, 1.0f, 1.0f)}, {"roughness", 0.3f}});

	Sphere matteSphere(1.0f, &matteMaterial, Transform::translate(Point3f(3.0f, 1.0f, 0.0f)));
	Sphere shinySphere(1.0f, &shinyMaterial, Transform::translate(Point3f(0.0f, 1.0f, 1.0f)));
	Sphere groundSphere(1000.0f, &groundMaterial, Transform::translate(Point3f(0.0f, -1000.0f, 0.0f)));

	// To raytrace more than one object at a time, we can put them into a group
	SurfaceGroup scene;
	scene.addChild(new Sphere(matteSphere));
	scene.addChild(new Sphere(shinySphere));
	scene.addChild(new Sphere(groundSphere));

	// Generate a ray for each pixel in the ray image
	for (auto y : range(rayImage.height()))
	{
		for (auto x : range(rayImage.width()))
		{
			auto ray = camera.generateRay(x + 0.5f, y + 0.5f);

			Color3f color;

			// TODO: Call recursiveColorFunction ``NumSamples'' times and average the
			// results. Assign the average color to ``color''
			
			//////////////////////////////////////////////////////////////////////////
			////Here is your implementation

			//////////////////////////////////////////////////////////////////////////

			rayImage(x, y) = color;
		}
	}

	message("Saving ray image to part_3_recursive_raytracing.png....\n");
	rayImage.save("part_3_recursive_raytracing.png");
}

void functionWithJSONParameters(const json & j)
{
	// Many of the constructors for ray tracing in DIRT
	// take a JSON object. This allows us to have a uniform interface for creating
	// these structures while allowing the constructors to retrieve
	// the necessary values from the JSON object. This will simplify
	// our code for writing a parser for reading scene files from disk.

	// Sometimes we may want to make a parameter optional, and take on some
	// default value if it is not specified.
	// Unfortunately, checking for a missing parameter using e.g. j["radius"]
	// will throw an exception if the parameter doesn't exist.
	// Instead, we can use j.value("name", defaultValue) to extract it.
	// This is what the constructors to Camera, Sphere, Quad, and Materials do.

	// TODO: Extract the parameters radius (default=1.f), and center (default=(0,0,0))

	float radius = 0.f;
	Vector3f center = Vector3f(0.f);
	message("The radius is: %f\n", radius);
	message("The center is:\n%s\n", center);
}

//////////////////////////////////////////////////////////////////////////
////If you decide to use DIRT for your final project, 
////you are required to finish the rest part of the tutorial (5 points) 
////to familiarize your self with the DIRT ray tracer 
void testColorAndImage()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Final project practice: Color & image tutorial  \n");
	message("--------------------------------------------------------\n");

	// The DIRT basecode provides a number of useful classes and functions
	// We will walk through the basic functionality of the most important
	// ones.

	// DIRT relies on the Eigen library for many of its basic classes.
	// Eigen is a widely-used library for linear algebra:
	//    http://eigen.tuxfamily.org/dox/GettingStarted.html
	//
	// The basic data structures in Eigen are matrices, vectors and arrays.
	// We introduce the convenience class Color3f (see color.h|cpp) to
	// represent RGB colors. This derives from the Eigen::Array3f class, a
	// fixed-size 1D array with 3 elements, which provides a number of useful
	// operations to manipulate arrays in bulk or access their elements.

	// A Color3f stores three floating-point values, one for red, green and
	// blue.
	Color3f red(1, 0, 0);
	Color3f blue(0, 0, 1);
	Color3f white = Color3f(1);     // This is the same as Color3f(1,1,1);

	// We can perform basic element-wise arithmatic on Colors:
	Color3f magenta = red + blue;
	Color3f stillRed = red * white;

	// TODO: Initialize the color pinkish to the arithmetic mean of white and red
    Color3f pinkish;

	// we can also print out colors to the console:
	// We can do this with cout, etc:
	cout << "white = \n" << white << endl;

	// But Dirt also provides some output functions (message, warning, error).
	// These operate like printf, but colorize the output based on the type of message.
	// These functions also have better support for Dirt's built-in types, so we
	// can output a Color3f like so:
	message("red =\n%s.\n", red);
	message("blue =\n%s.\n", blue);
	message("magenta =\n%s.\n", magenta);
	message("pinkish =\n%s.\n", pinkish);
	message("stillRed =\n%s.\n", stillRed);

	// We can also access the individual elements of the color by channel index:
	message("Red channel of pinkish is: %f\n", pinkish[0]);

	// or by channel name: r(), g(), b())

	// TODO: Print out the green channel of pinkish using the named accessor
	message("Green channel of pinkish is: %f\n", 0.0f);

	message("Blue channel of stillRed is: %f\n", stillRed[1]);

	pinkish.r() *= 2.f;

	message("After scaling by 2, red channel of pinkish is: %f\n", pinkish.r());

	// The Color3f class provides a few additional operations which are useful
	// specifically for manipulating colors, see the color.h header file.

	// TODO: Compute and print the luminance of pinkish. Look at color.h to see what method you might need
	message("The luminance of pinkish is: %f\n", 0.0f);

	// DIRT also provides the Image3f class (see image.h|cpp) to load, store,
	// manipulate, and write images.

	// Image3f is just a dynamically allocated 2D array of pixels. It
	// derives from the Eigen::Array class, which is a generic 2D array
	// container of arbitrary size.

	// Here we construct an empty image that is 200 pixels across, and
	// 100 pixels tall:
	auto image1 = Image3f(200, 100);

	// In the case of Image3f, each array element (pixel) is a Color3f, which
	// is itself a 3-element array.

	// We can access individual pixels of an Image3f using the (x,y) operator:
	image1(5,10) = white;       // This sets the pixel to white


	// The file common.h defines a simple linear interpolation function: lerp
	// which allows us to specify two values, a and b, and an interpolation
	// parameter t. This function is a template, which means it will work with
	// any type as long as (in this case) we can add them and multiply by a scalar.
	// Just as we could interpolate between two scalar values, we can also use
	// it to interpolate between two colors:

	message("25%% of the way from blue to red is:\n%s.\n", lerp(blue, red, 0.25));


	// Now, let's populate the colors of an entire image, and write it to a PNG
	// file.

	Image3f gradient(200,100);

	// TODO: Populate and output the gradient image
	// First, loop over all rows, and then columns of an image.
	// Set the red component of a pixel's color to vary linearly from 0 at the
	// leftmost pixel to 1 at the rightmost pixel; and the green component to
	// vary from 0 at the topmost pixel to 1 at the bottommost pixel. The blue
	// component should be 0 for all pixels.

	// After populating the pixel colors, look at the member functions of
	// Image3f, and call a function to save the gradient image out to the file
	// "gradient.png".

	message("Creating gradient image.\n");

	put_your_code_here("Populate an image with a color gradient and write to disk.");
    message("Saving image \"gradient.png\" ...\n");

	// Now, we will load an image, modify it, and save it back to disk.
	Image3f image;

	// TODO: Load the image scenes/00_preliminaries/cornellbox.png into the ``image'' variable
	message("Loading image cornellbox.png ...\n");
	put_your_code_here("Load the image \"scenes/00_preliminaries/cornellbox.png\".");
    // Hint: Take a look at Image3f::load
    // Keep in mind filenames are interpreted relative to your current
    // working directory

	// TODO: Convert the image to grayscale. Loop over every pixel and convert
	// it to grayscale by replacing every pixel with its luminance
	message("Converting image to grayscale....\n");
	put_your_code_here("Convert the image to grayscale.");

	// TODO: Save the image to scenes/00_preliminaries/cornell_grayscale.png
	// Hint: Take a look at Image3f::save
	message("Saving image cornell_grayscale.png....\n");
	put_your_code_here("Save the image to \"cornell_grayscale.png\".");

	message("Done!\n");
}

void testVectorsAndMatrices()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Final project practice: Vectors                 \n");
	message("--------------------------------------------------------\n");

	// DIRT also provides linear algebra helper classes based on Eigen.
	// We distinguish between points, vectors and normals. For example, to
	// create a vector with three float (f) elements, you can write
	//
	//      Vector3f vector(a, b, c);
	//
	// You can also use
	//
	//      Vector3f vector = Vector3f(a, b, c);
	//
	// or simply:
	//
	//      auto vector = Vector3f(a, b, c);
	//
	// and similarly for points and normals. Use whichever form you prefer.

	// Let's create some vectors, points and normals
	Vector3f vector = Vector3f(-0.1, 0.2, -0.3);
	Point3f point   =  Point3f( 0.5, 0.25, -0.123);
	Normal3f normal = Normal3f(-1.0f, 1.0f, 2.0f).normalized();

	message("vector =\n%s.\n", vector);
	message("point =\n%s.\n", point);
	message("normal =\n%s.\n", normal);

	// If you want to extract the first n elements of a vector, you can use the head<n>() function. For example:
	message("The first two elements of vector are\n%s.\n", vector.head<2>());

	// You can access specific components using x(), y(), z() and w().
	// TODO: Output the z coordinate of the normal
	message("The z coordinate of normal is %s.\n", 0.0f);

	// We will also make use of rays, which represent an origin and a direction:
	Ray3f ray = Ray3f(Point3f(0.5f, 2.0f, -3.0f),
		              Vector3f(-0.25f, -0.5f, 0.3f));

	// Let's print some info about our ray
	message("The origin of ray is \n%s.\n", ray.o);
	message("The direction of ray is \n%s.\n", ray.d);

	// We will also have access to matrices. Matrices are named according to their number of elements
	// For example, a Matrix4f is a 4x4 matrix.
	// To fill a matrix, we can use the convenient notation of
	//
	//      matrix << a, b, c, d,
	//                e, f, g, h,
	//                i, j, k, l,
	//                m, n, o, p;
	//
	// White space doesn't matter here, but pay attention to commas and semicolons.
	Matrix4f matrix;
	matrix <<
	    -0.5, -0.4, -0.2, -1.0,
		 0.1, -0.5,  0.8,  0.1,
		-0.5,  0.7,  0.5, -0.6,
		 0.0,  0.0,  0.0,  1.0;

	// Similar to vectors, you can extract parts of the matrix. For example, to get the top left 3x3 part of a matrix, you can use
	message("The top-left 3x3 submatrix of matrix is\n%s.\n", matrix.topLeftCorner<3, 3>());
}

void testJSON()
{
	// DIRT also includes a C++ library (https://github.com/nlohmann/json)
	// for parsing and manipulating JSON data.
	//
	// JSON is a human-readible data interchange format for expressing
	// attribute-value pairs. You can read more about it here:
	//      https://en.wikipedia.org/wiki/JSON
	//      https://www.json.org/
	//
	// In DIRT, we will use it for two purposes:
	//  1) As a generic way to pass named parameters to functions
	//  2) As a way to specify and load text-based scene files

	cout << endl;
	message("--------------------------------------------------------\n");
	message("Final project practice: passing data using JSON \n");
	message("--------------------------------------------------------\n");

	float      f(2.f);
	string     s("a text string");
	Color3f  c3f(1.0f, .25f, .5f);
	Vector3f v3f(2,3,4);
	Vector4f v4f(2,3,4,5);
	Normal3f n3f(2,3,4);
	message("Original DIRT data:\nf = %f,\ns = %s,\nc3f = \n%s,\nv3f = \n%s,\nv4f = \n%s,\nn3f = \n%s.\n", f, s, c3f, v3f, v4f, n3f);

	// All the basic DIRT data-types can easily be stored in a JSON object
	json j;
	j["my float"]   = f;
	j["my string"]  = s;
	j["my color"]   = c3f;
	j["my vector3"] = v3f;
	j["my vector4"] = v4f;
	j["my normal"]  = n3f;
	message("The JSON object contains:\n%s.\n", j.dump(4));

	// We can also read these structures back out of the JSON object
	float      f2 = j["my float"];
	string     s2 = j["my string"];
	Color3f  c3f2 = j["my color3"];
	Vector3f v3f2 = j["my vector3"];
	Vector4f v4f2 = j["my vector4"];
	Normal3f n3f2 = j["my normal"];

	message("Retrieved DIRT data:\nf2 = %f,\ns2 = %s,\nc3f2 = \n%s,\nv3f2 = \n%s,\nv4f2 = \n%s,\nn3f2 = \n%s.\n",
		    f2, s2, c3f2, v3f2, v4f2, n3f2);
	// TODO: There is a bug in the code above, and c3f2 doesn't have the same value as the original c3f. Fix it.

	// Now we will pass a json object in place of explicit parameters to
	// a function:
	json parameters = {{"radius", 2.3f}};
	functionWithJSONParameters(parameters);
}

// Next, we will generate the same image, but using the Camera class
void testCameraClassImage()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Final project practice: Camera class generateRay\n");
	message("--------------------------------------------------------\n");

	// Setup the output image
	Image3f rayImage(200, 100);

	// Set up a camera with some reasonable parameters, using JSON
	// TODO: Look in camera.h and implement the camera constructor
	Camera camera({
		              {"vfov", 90.f},
		              {"resolution", Vector2i(rayImage.width(), rayImage.height())},
		              {"fdist", 1.f}
	              });

	// loop over all pixels and ask the camera to generate a ray
	for (auto y : range(rayImage.height()))
	{
		for (auto x : range(rayImage.width()))
		{
			// TODO: Look in camera.h and implement camera.generateRay

			// We add 0.5 to the pixel coordinate to center the ray within the pixel
			auto ray = camera.generateRay(x + 0.5f, y + 0.5f);
			rayImage(x, y) = rayToColor(ray);
		}
	}

	message("Saving ray image to 01_camera_ray_image.png....\n");
	rayImage.save("01_camera_ray_image.png");
}

// Finally, we will allow our camera to be positioned and oriented using a Transform
void testXformedCameraImage()
{
	cout << endl;
	message("--------------------------------------------------------\n");
	message("Final project practice: Transformed camera      \n");
	message("--------------------------------------------------------\n");

	// Setup the output image
	Image3f rayImage(200, 100);

	// Set up a camera with some reasonable parameters
	// TODO: Look in camera.h and implement the camera constructor
	Camera camera({
		              {"vfov", 90.f},
		              {"resolution", Vector2i(rayImage.width(), rayImage.height())},
		              {"fdist", 1.f},
		              {"transform", {{"from", {5.0, 15.0, -25.0}},
		                             {"to",   {0.0,  0.0,   0.0}},
		                             {"up",   {0.0,  1.0,   0.0}}} },
	              });

	// Generate a ray for each pixel in the ray image
	for (auto y : range(rayImage.height()))
	{
		for (auto x : range(rayImage.width()))
		{
			// TODO: Look in camera.h and implement camera.generateRay

			// Make sure to take the camera transform into account!

			// We add 0.5 to the pixel coordinate to center the ray within the pixel
			auto ray = camera.generateRay(x + 0.5f, y + 0.5f);
			rayImage(x, y) = rayToColor(ray);
		}
	}

	message("Saving ray image to 01_xformed_camera_ray_image.png....\n");
	rayImage.save("01_xformed_camera_ray_image.png");
}

int main(int argc, char **argv)
{
	//////////////////////////////////////////////////////////////////////////
	////Assignment 4, generating rays, ray-sphere intersection, and recursive ray tracing
    testManualCameraImage();
	testRaySphereIntersection();
	testRecursiveRaytracing();

	//////////////////////////////////////////////////////////////////////////
	////further tutorials for the ray tracing final project
	//testColorAndImage();
	//testVectorsAndMatrices();
	//testJSON();
	//testCameraClassImage();
	//testXformedCameraImage();

    return 0;
}
