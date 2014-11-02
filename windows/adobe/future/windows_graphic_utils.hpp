/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIN32_GRAPHIC_UTILS_HPP
#define ADOBE_WIN32_GRAPHIC_UTILS_HPP

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1

#include <windows.h>

#include <adobe/config.hpp>

#include <boost/gil/typedefs.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

    HBITMAP to_bitmap(const boost::gil::rgba8_image_t& image);

/****************************************************************************************************/

HCURSOR to_cursor(HBITMAP bitmap, int hotspot_x, int hotspot_y);

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
// ADOBE_WIN32_GRAPHIC_UTILS_HPP
#endif

/****************************************************************************************************/
