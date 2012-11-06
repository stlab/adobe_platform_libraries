/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
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
