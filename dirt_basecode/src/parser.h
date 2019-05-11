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

#include "transform.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4706)
#endif
#include <json.hpp>
#if defined(_MSC_VER)
#pragma warning(pop) 
#endif

using json = nlohmann::json;

// json loading
json loadJSON(const string & filename);

// command line specification
struct CommandLine
{
    // description of command line argument
    struct Argument
    {
        string name;                    //!< complete name
        string flag;                    //!< flag string name
        string desc;                    //!< description
        const std::type_info & type;    //!< supported: int, double, bool (flag), string
        bool opt;                       //!< whether it is an optional argument
        json def;                       //!< default value
    };

    // parameters
    string progname;                //!< program name
    vector<Argument>    options;    //!< program options (flags)
    vector<Argument>    arguments;  //!< program arguments

    CommandLine(const string &progname, const vector<Argument>&options,
    	        const vector<Argument>&arguments) :
        progname(progname), options(options), arguments(arguments)
    {
    }

    // parsing command line arguments to json
    json parse(const vector<string> & args) const;
    json parse(int argc, char** argv) const;

    void printUsageAndQuit(const std::string & msg = {}) const;
};




namespace Eigen
{

//! parse a fixed-size Eigen matrix or vector
template <typename _Scalar, int _Rows, int _Cols>
inline void from_json(const json & j, Matrix<_Scalar, _Rows, _Cols> & v)
{
    if (j.size() == 1)
    {
        v.setConstant(j.get<_Scalar>());
        warning("Incorrect array size when trying to parse an Eigen::Matrix. "
                "Expecting %d values but only found a single scalar. "
                "Creating a %d x %d Eigen::Matrix of all \"%f\"s.\n", v.size(), _Rows, _Cols, (float)j.get<_Scalar>());
        return;
    }
    else if (v.size() != (int)j.size())
    {
        error("Incorrect array size when trying to parse an Eigen::Matrix. "
              "Expecting %d values but found %d.\n", v.size(), j.size());
    }

    for (auto i : range(j.size()))
        v.data()[i] = j.at(i).get<_Scalar>();
}

//! parse a fixed-size Eigen array
template <typename _Scalar, int _Rows, int _Cols>
inline void from_json(const json & j, Array<_Scalar, _Rows, _Cols> & v)
{
    if (j.size() == 1)
    {
        v.setConstant(j.get<_Scalar>());
        warning("Incorrect array size when trying to parse an Eigen::Array. "
                "Expecting %d values but only found a single scalar. "
                "Creating a %d x %d Eigen::Array of all \"%f\"s.\n", v.size(), _Rows, _Cols, (float)j.get<_Scalar>());
        return;
    }
    else if (v.size() != (int)j.size())
    {
        error("Incorrect array size when trying to parse an Eigen::Array. "
              "Expecting %d values but found %d.\n", v.size(), j.size());
    }

    for (auto i : range(j.size()))
        v.data()[i] = j.at(i).get<_Scalar>();
}


//! parse a fixed-size Eigen matrix or vector
template <typename _Scalar, int _Rows, int _Cols>
inline void to_json(json & j, const Matrix<_Scalar, _Rows, _Cols> & v)
{
    j = std::vector<_Scalar>(v.data(), v.data() + v.size());
}

//! parse a fixed-size Eigen array
template <typename _Scalar, int _Rows, int _Cols>
inline void to_json(json & j, const Array<_Scalar, _Rows, _Cols> & v)
{
    j = std::vector<_Scalar>(v.data(), v.data() + v.size());
}

} // namespace Eigen



//! read a dirt Transform from json
void from_json(const json & j, Transform & t);

//! write a dirt transform to json
inline void to_json(json & j, const Transform & t)
{
	to_json(j, t.getMatrix());
}

