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

    ------------------------------------------------------------------------

    This file is based off the Vector classes in Nori:

    Copyright (c) 2015 by Wenzel Jakob, Romain Prévost
*/

#pragma once

#include "common.h"

/* ===================================================================
    This file contains a few templates and specializations, which
    provide 2/3D points, vectors, and normals over different
    underlying data types. Points, vectors, and normals are distinct
    in Dirt, because they transform differently under homogeneous
    coordinate transformations.
 * =================================================================== */

//! Generic N-dimensional vector data structure based on Eigen::Matrix
template <typename _Scalar, int _Dimension>
struct Vector : public Eigen::Matrix<_Scalar, _Dimension, 1>
{
public:
    enum
    {
        Dimension = _Dimension
    };

    using Scalar = _Scalar;
    using Base = Eigen::Matrix<Scalar, Dimension, 1>;
    using VectorType = Vector<Scalar, Dimension>;
    using PointType = Point<Scalar, Dimension>;

    //! Create a new vector with constant component vlaues
    Vector(Scalar value = (Scalar) 0)
    {
        Base::setConstant(value);
    }

    //! Create a new 2D vector (type error if @c Dimension != 2)
    Vector(Scalar x, Scalar y) : Base(x, y) {}

    //! Create a new 3D vector (type error if @c Dimension != 3)
    Vector(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}

    //! Create a new 4D vector (type error if @c Dimension != 4)
    Vector(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w) {}

    //! Construct a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Vector(const Eigen::MatrixBase<Derived>&p)
        : Base(p) {}

    //! Assign a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived>
    Vector & operator=(const Eigen::MatrixBase<Derived>&p)
    {
        this->Base::operator=(p);
        return *this;
    }
};

//! Generic N-dimensional point data structure based on Eigen::Matrix
template <typename _Scalar, int _Dimension>
struct Point : public Eigen::Matrix<_Scalar, _Dimension, 1>
{
public:
    enum
    {
        Dimension = _Dimension
    };

    using Scalar = _Scalar;
    using Base = Eigen::Matrix<Scalar, Dimension, 1>;
    using VectorType = Vector<Scalar, Dimension>;
    using PointType = Point<Scalar, Dimension>;

    //! Create a new point with constant component vlaues
    Point(Scalar value = (Scalar) 0)
    {
        Base::setConstant(value);
    }

    //! Create a new 2D point (type error if @c Dimension != 2)
    Point(Scalar x, Scalar y) : Base(x, y) {}

    //! Create a new 3D point (type error if @c Dimension != 3)
    Point(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}

    //! Create a new 4D point (type error if @c Dimension != 4)
    Point(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w) {}

    //! Construct a point from MatrixBase (needed to play nice with Eigen)
    template <typename Derived>
    Point(const Eigen::MatrixBase<Derived>&p) : Base(p) {}

    //! Assign a point from MatrixBase (needed to play nice with Eigen)
    template <typename Derived>
    Point & operator=(const Eigen::MatrixBase<Derived>&p)
    {
        this->Base::operator=(p);
        return *this;
    }
};

//! 3-dimensional surface normal representation
struct Normal3f : public Eigen::Matrix<float, 3, 1>
{
public:
    enum
    {
        Dimension = 3
    };

    using Scalar = float;
    using Base = Eigen::Matrix<Scalar, Dimension, 1>;
    using VectorType = Vector<Scalar, Dimension>;
    using PointType = Point<Scalar, Dimension>;


    //! Create a new normal with constant component vlaues
    Normal3f(Scalar value = 0.0f)
    {
        Base::setConstant(value);
    }

    //! Create a new 3D normal
    Normal3f(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}

    //! Construct a normal from MatrixBase (needed to play nice with Eigen)
    template <typename Derived>
    Normal3f(const Eigen::MatrixBase<Derived>&p) : Base(p) {}

    //! Assign a normal from MatrixBase (needed to play nice with Eigen)
    template <typename Derived>
    Normal3f & operator=(const Eigen::MatrixBase<Derived>&p)
    {
        this->Base::operator=(p);
        return *this;
    }
};


/// A 3D axis-aligned bounding box.
/*!
    Based on Eigen::AlignedBox with additional functionality.
*/
struct AABB3f : public Eigen::AlignedBox<float, 3>
{
    using Base = Eigen::AlignedBox<float, 3>;
    using VectorType = typename Base::VectorType;

    /// Default constructor initializing a null box.
    AABB3f() : Base() {}

    template<typename OtherVectorType1, typename OtherVectorType2>
    AABB3f(const OtherVectorType1& _min, const OtherVectorType2& _max)
        : Base(_min, _max) {}

    /// Construct an AABB3f from AlignedBox (needed to play nice with Eigen)
    template <typename DerivedS, int DerivedD>
    AABB3f(const Eigen::AlignedBox<DerivedS, DerivedD> & p)
        : Base(p) {}

    /// Assign an AABB3f from AlignedBox (needed to play nice with Eigen)
    template <typename DerivedS, int DerivedD>
    AABB3f & operator=(const Eigen::AlignedBox<DerivedS, DerivedD> & p)
    {
        this->Base::operator=(p);
        return *this;
    }

    /// Choose the min or max corner by index
    const VectorType & operator[](int i) const
    {
        assert(i == 0 || i == 1);
        return (&this->Base::m_min)[i];
    }

    /// Choose the min or max corner by index
    VectorType & operator[](int i)
    {
        assert(i == 0 || i == 1);
        return (&this->Base::m_min)[i];
    }

    /// Calculate the n-1 dimensional volume of the boundary
    float surfaceArea() const
    {
        auto d = Base::diagonal();
        float result = 0.0f;
        for (auto i : range(3))
        {
            float term = 1.0f;
            for (auto j : range(3))
            {
                if (i == j)
                    continue;
                term *= d[j];
            }
            result += term;
        }
        return 2.0f * result;
    }

    /// Return the dimension index with the largest associated side length
    int majorAxis() const
    {
        auto d = Base::diagonal();
        int largest = 0;
        for (auto i : range(3))
            if (d[i] > d[largest])
                largest = i;
        return largest;
    }
};

using Eigen::Matrix2f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;
