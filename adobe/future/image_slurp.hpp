/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_IMAGE_SLURP_HPP
#define ADOBE_IMAGE_SLURP_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/filesystem/path.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/typedefs.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

///
/// Sucks up an image from a file on disk into a GIL image_t. This leverages the ASL IO code for image
/// importing in order to detect and import the file according to its proper format.
///
/// \param path  Relative path to the image; the rest of the path lookup takes place via find_resource
/// \param image Image data to be filled with the loaded image.
///

void image_slurp(const boost::filesystem::path& path, boost::gil::rgba8_image_t& image);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
