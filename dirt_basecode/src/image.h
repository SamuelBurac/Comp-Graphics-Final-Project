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

#include "color.h"

//! A floating-point RGB image
class Image3f : public Eigen::Array<Color3f,Eigen::Dynamic,Eigen::Dynamic>
{
    using Base = Eigen::Array<Color3f,Eigen::Dynamic,Eigen::Dynamic>;

public:
    //! Default constructor (empty image)
    Image3f() : Base() {}
    /*!
     * @brief       Size Constructor (sets width and height)
     * @param w     The width of the image
     * @param h     The height of the image
     */
    Image3f(int w, int h) : Base(w, h) {}

    /*!
     * @brief       Construct an image of a fixed size and initialize all pixels
     * @param w     The width of the image
     * @param h     The height of the image
     * @param v     The Color to set all pixels
     */
    Image3f(int w, int h, const Color3f &v) : Base(w,h) {setConstant(v);}

	//@{ Functions needed to interfacing with rest of Eigen
    Image3f(const Base & other) : Base(other) {}

    //! This constructor allows you to construct an Image3f from Eigen expressions
    template <typename OtherDerived>
    Image3f(const Eigen::ArrayBase<OtherDerived>& other) : Base(other) { }

    //! This method allows you to assign Eigen expressions to an Image3f
    template <typename OtherDerived>
    Image3f& operator=(const Eigen::ArrayBase <OtherDerived>& other)
    {
        this->Base::operator=(other);
        return *this;
    }
    //@}

    int width() const {return (int)rows();}
    int height() const {return (int)cols();}

	/*!
	 * @brief			Load an image from file
	 * @param filename	The filename
	 * @return 			True if the file loaded successfully
	 */
    bool load(const std::string & filename);

	/*!
	 * @brief			Save an image to the specified filename
	 * @param filename 	The filename to save to
	 * @param gain 		The multiplicative gain to apply to pixel values before saving
	 * @return 			True if the file saved successfully
	 */
    bool save(const std::string & filename, float gain = 1.0f);
};
