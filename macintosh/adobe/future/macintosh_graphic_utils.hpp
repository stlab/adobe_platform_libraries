/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_GRAPHIC_UTILS_HPP
#define ADOBE_MACINTOSH_GRAPHIC_UTILS_HPP

/****************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>
#include <boost/gil/gil_all.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

// Unfortunately, there are several functions and other support state variables we need to retain
// in order to feed a CGImageRef with the information it needs. Sadly, while all this is handled
// internally for the client, they'll need to interact with the system via a wrapped type.

/*
    WARNING (fbrereto) : Due to the way Mac OS X reference counts (in this case) CGImageRef objects,
                         it is possible to abuse this mechanism by retaining a CGImageRef and then
                         trying to delete the cg_image_t. The issue here is the state information
                         that is needed by the OS to construct the CGImageRef is not guaranteed to
                         be used immediately -- it's possible that the implementation may do a
                         lazy copy of the image data well after the CGImageRef has been created. If
                         this is the case, the scope of the cg_image_t must meet or exceed that of
                         the CGImageRef that it creates, otherwise you'll be diving off a bad
                         pointer.
*/

typedef struct __lame_cg_image_wrapper_t* cg_image_t;

cg_image_t   make_cg_image(const boost::gil::rgba8_image_t& image);
void         destroy_cg_image(cg_image_t image);

::CGImageRef to_CGImageRef(cg_image_t image);

/****************************************************************************************************/

// delete_ptr specialization for the cg_image_t wrapped type so it can be used with auto_resource

ADOBE_DELETE_PTR_SPECIALIZATION(cg_image_t, destroy_cg_image)

/****************************************************************************************************/

void paint_control_with_image(::HIViewRef control, const boost::gil::rgba8_image_t& image);

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_MACINTOSH_GRAPHIC_UTILS_HPP
#endif

/****************************************************************************************************/
