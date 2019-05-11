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

#include "bbh.h"

bool aabbIntersect(const AABB3f &bounds, const Ray3f &ray)
{
    // TODO: Assignment 6: Implement ray-AABB intersection
    //       Read chapter 2 for hints on how to do this
    //       If the ray hits the AABB within the interval of
    //       minT/maxT, return true; otherwise, return false
    float minT = ray.mint;
    float maxT = ray.maxt;

    put_your_code_here("Assignment 6: Insert your ray-aabb intersection code here");
    return false;
}

BBHNode::BBHNode(std::vector<Surface *> primitives)
: m_left(nullptr),
  m_right(nullptr)
{
    // TODO: Assignment 6: Implement BVH construction, following the book in chapter 2.
    //       Hints:
    //          -- To get a random number in [0, n-1], you can use int(randf()*n)
    //          -- To sort a list of surfaces based on some sorting criterion, you can use std::sort and lambdas, like so:
    //
    //                         std::sort(primitives.begin(), primitives.end(), [&](const Surface *a, const Surface *b) {
    //                             return isASmallerThanB(a, b);
    //                         });
    //
    //             For example, to sort them based on centroid along an axis, you could replace `return isASmallerThanB(a, b);` with
    //
    //                  return a->worldBBox().center()[axis] < b->worldBBox().center()[axis]
    //
    //          -- To split a list (in C++: a 'vector') of things into two lists so that the first list has the first
    //             k elements of the original list, and the other list has the rest of the elements, you can do
    //                  std::vector<Surface*> listA, listB
    //                  listA.insert(listA.end(), originalList.begin(), originalList.begin() + k);
    //                  listB.insert(listB.end(), originalList.begin() + k, originalList.end());
    //
    //          -- After construction, you need to compute the bounds of this BVH node and assign it to m_bounds
    //          -- You can get the bounding box of a surface using surf->worldBBox();
    //          -- You can get the bounding box that contains boxes boxA and boxB using boxA.merged(boxB);
    put_your_code_here("Assignment 6: Insert your BVH construction code here");
}

BBHNode::~BBHNode()
{
}

bool BBHNode::intersect(const Ray3f &ray, HitInfo &hit) const
{
    // TODO: Assignment 6: Implement BVH intersection, following the book in chapter 2.
    put_your_code_here("Assignment 6: Insert your BVH intersection code here");
    return false;
}

// BBH Method Definitions
BBH::BBH(const Scene & scene, const json & j) : SurfaceGroup(scene, j)
{
    // These values aren't used in the base code right now - but you can use these for when you want to extend
    // the basic BVH functionality

    int maxSurfsInNode = 10;
    maxSurfsInNode = j.value("maxPrimsInNode", maxSurfsInNode);

    string sm("sah");
    sm = j.value("splitMethod", sm);
    if (sm == "sah") {
        // Surface-area heuristic
    } else if (sm == "middle") {
        // Split at the center of the bounding box
    } else if (sm == "equal") {
        // Split so that an equal number of objects are on either side
    }
}

BBH::~BBH()
{
    if (m_root)
        delete m_root;
}

void BBH::build()
{
    if (!m_surfaces.empty())
        m_root = new BBHNode(m_surfaces);
    else
        m_root = nullptr;
}

bool BBH::intersect(const Ray3f &ray, HitInfo &hit) const
{
    if (m_root)
        return m_root->intersect(ray, hit);
    else
        return false;
}
