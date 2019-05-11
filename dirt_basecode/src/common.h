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

#if defined(_MSC_VER)
/* Disable some warnings on MSVC++ */
#pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
#define WIN32_LEAN_AND_MEAN     /* Don't ever include MFC on Windows */
#define NOMINMAX                /* Don't override min/max */
#endif

#include <rang.hpp>            // for colorful console output
#include <string>              // for string
#include <stdio.h>             // for size_t
#include <tinyformat.h>        // for format
#include <iostream>            // for string, cout, flush, ostream, cerr, cin
#include <map>                 // for map
#include <set>                 // for set
#include <stdexcept>           // for runtime_error
#include <string>              // for operator+, basic_string
#include <utility>             // for make_pair, pair
#include <vector>              // for vector

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <Eigen/Core>
#include <Eigen/Geometry>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// bringing stand library objects in scope
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::set;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::flush;


// "Ray epsilon": relative error threshold for ray intersection computations
#define Epsilon 0.001f

// A few useful constants
#undef M_PI

#define M_PI         3.14159265358979323846f
#define INV_PI       0.31830988618379067154f
#define INV_TWOPI    0.15915494309189533577f
#define INV_FOURPI   0.07957747154594766788f
#define SQRT_TWO     1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f


// math utilities --------------------------
// max
inline int max(int a, int b)
{
    return (a > b) ? a : b;
}
inline float max(float a, float b)
{
    return (a > b) ? a : b;
}
// min
inline int min(int a, int b)
{
    return (a < b) ? a : b;
}
inline float min(float a, float b)
{
    return (a < b) ? a : b;
}
// clamp (sets x between m and M)
inline int clamp(int x, int m, int M)
{
    return min(M,max(m,x));
}
inline float clamp(float x, float m, float M)
{
    return min(M,max(m,x));
}


//! Error signaling unimplemented features
#define put_your_code_here(txt) \
    do { \
        static bool been_here = false; if (!been_here) { \
            been_here = true; \
            warning("%s() not (fully) implemented at %s:%d.\n    msg: %s\n", __FUNCTION__, __FILE__, __LINE__, txt); \
        } } while (0);

//! Prints a message (printf style) and flushes the output (useful during debugging)
template <typename ... Args>
inline void message(const char *msg, const Args &... args)
{
    string msg2 = tfm::format(("[Info]: " + string(msg)).c_str(), args...);
    cout << rang::fg::green << msg2
         << rang::style::reset << rang::fg::reset << flush;
}

//! Prints an error message (printf style), flushes the output (useful during debugging) and asserts
template <typename ... Args>
inline void error(const char *msg, const Args &... args)
{
    string msg2 = tfm::format(("[Error]: " + string(msg)).c_str(), args ...);
    cerr << rang::fg::red << rang::style::bold << msg2
         << rang::style::reset << rang::fg::reset << flush;
}

//! Prints an error message (printf style), flushes the output (useful during debugging) and asserts
template <typename ... Args>
inline void warning(const char *msg, const Args &... args)
{
    string msg2 = tfm::format(("[Warning]: " + string(msg)).c_str(), args...);
    cout << rang::fg::yellow << rang::style::bold << msg2
         << rang::style::reset << rang::fg::reset << flush;
}


/*!
 * @brief Python-style range: iterates from min to max in range-based for loops
 *
 * To use:
 *  @code
 *     for(int i = 0; i < 100; i++) { ... }             // old way
 *     for(auto i : range(100))     { ... }             // new way
 *
 *     for(int i = 10; i < 100; i+=2)  { ... }          // old way
 *     for(auto i : range(10, 100, 2)) { ... }          // new way
 *
 *     for(float i = 3.5f; i > 1.5f; i-=0.01f) { ... } // old way
 *     for(auto i : range(3.5f, 1.5f, -0.01f)) { ... } // new way
 * @endcode
*/
template<typename T>
class Range
{
public:
    class Iterator
    {
    public:
        Iterator(T pos, T step) : m_pos(pos), m_step(step) {}

        bool operator!=(const Iterator &o) const { return (o.m_pos - m_pos) * m_step > T(0);}
        Iterator &operator++() {m_pos += m_step; return *this;}
        Iterator operator++(int) {Iterator copy(*this); operator++(); return copy;}
        T operator*() const {return m_pos;}
    private:
        T m_pos, m_step;
    };

    Range(T start, T end, T step = T(1))
        : m_start(start), m_end(end), m_step(step) {}

    Iterator begin() const {return Iterator(m_start, m_step);}
    Iterator end() const {return Iterator(m_end, m_step);}

private:
    T m_start, m_end, m_step;
};

template<typename T>
Range<T> range(T end) {return Range<T>(T(0), end, T(1));}

template<typename T>
Range<T> range(T start, T end, T step = T(1)) {return Range<T>(start, end, step);}


/*!
 * @brief Linear interpolation.
 *
 * Linearly interpolates between \a a and \a b, using parameter \a t.
 *
 * @tparam T    type for start and end points, and return value
 * @tparam S    type for interpolation parameter
 * @param a     Start point
 * @param b     End point
 * @param t     A blending factor of \a a and \a b.
 * @return      Linear interpolation of \a a and \a b -
 *              a value between \a a and \a b if \a t is between 0 and 1.
 */
template <typename T, typename S>
inline T
lerp(T a, T b, S t)
{
    return T((S(1)-t) * a + t * b);
}

//! Convert radians to degrees
inline float rad2deg(float value) { return value * (180.0f / M_PI); }

//! Convert degrees to radians
inline float deg2rad(float value) { return value * (M_PI / 180.0f); }


//! Convert a time value in milliseconds into a human-readable string
extern std::string timeString(double time, int precision = 2);

//! Convert a memory amount in bytes into a human-readable string
extern std::string memString(size_t size, int precision = 2);

//! Simple exception class, which stores a human-readable error description
class DirtException: public std::runtime_error
{
public:
    //! Variadic template constructor to support printf-style arguments
    template <typename ... Args>
    DirtException(const char *fmt, const Args &... args)
        : std::runtime_error(tfm::format(fmt, args ...))
    {
    }
};

// Acceleration structure stats parameters
extern uint64_t intersection_tests;
extern uint64_t rays_traced;

#define INCREMENT_INTERSECTION_TESTS intersection_tests++
#define INCREMENT_TRACED_RAYS rays_traced++
