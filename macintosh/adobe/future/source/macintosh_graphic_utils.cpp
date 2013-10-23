/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/macintosh_controls.hpp>
#include <adobe/future/macintosh_error.hpp>
#include <adobe/future/macintosh_graphic_utils.hpp>

#include <boost/static_assert.hpp>
#include <boost/noncopyable.hpp>

#include <CoreGraphics/CGDataProvider.h>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

struct cg_image_support_t : boost::noncopyable
{
    typedef boost::gil::rgba8_view_t view_type;
    typedef view_type::iterator      iterator;

    typedef adobe::auto_resource< ::CGDataProviderRef > auto_provider_t;
    typedef adobe::auto_resource< ::CGColorSpaceRef >   auto_colorspace_t;
    typedef adobe::auto_resource< ::CGImageRef >        auto_image_ref_t;

    explicit cg_image_support_t(const view_type& image);

    view_type         view_m;
    char*             first_m;
    auto_image_ref_t  image_ref_m;
    auto_provider_t   provider_m;
    auto_colorspace_t colorspace_m;
};

/****************************************************************************************************/

off_t provider_skip_bytes(void*       info,
                         off_t        count)
{
    cg_image_support_t& holder(*reinterpret_cast<cg_image_support_t*>(info));

    assert (count % 4 == 0);

    holder.first_m += count;
    
    return count;
}

/****************************************************************************************************/

std::size_t provider_get_bytes(void*       info,
                               void*       buffer,
                               std::size_t count)
{
    typedef cg_image_support_t::iterator iterator;

    assert (count % 4 == 0);

    cg_image_support_t& holder(*reinterpret_cast<cg_image_support_t*>(info));

    std::memmove(buffer, holder.first_m, count);

    provider_skip_bytes(info, count);

    return count;
}

/****************************************************************************************************/

void provider_rewind_bytes(void* info)
{
    cg_image_support_t& holder(*reinterpret_cast<cg_image_support_t*>(info));

    holder.first_m = reinterpret_cast<char*>(&(*holder.view_m.begin()));
}

/****************************************************************************************************/

cg_image_support_t::cg_image_support_t(const view_type& view) :
    view_m(view)
{
    first_m = reinterpret_cast<char*>(&(*view_m.begin()));

    CGDataProviderSequentialCallbacks cg_callbacks;

    cg_callbacks.version = 0;
    cg_callbacks.getBytes = &provider_get_bytes;
    cg_callbacks.skipForward = &provider_skip_bytes;
    cg_callbacks.rewind = &provider_rewind_bytes;
    cg_callbacks.releaseInfo = NULL;

    provider_m.reset(CGDataProviderCreateSequential(this, &cg_callbacks));

    colorspace_m.reset(::CGColorSpaceCreateDeviceRGB());

    std::size_t image_width(view_m.width());
    std::size_t image_height(view_m.height());
    std::size_t bits_per_component(8);
    std::size_t bits_per_pixel(32);
    std::size_t bytes_per_row(4 * image_width);

    image_ref_m.reset(::CGImageCreate(
        image_width,
        image_height,
        bits_per_component,
        bits_per_pixel,
        bytes_per_row,
        colorspace_m.get(),
        kCGImageAlphaLast/* | kCGBitmapByteOrderDefault*/,
        provider_m.get(),
        0,
        false, /* no pixel interpolation */
        kCGRenderingIntentDefault /* no gamut correction */
    ));
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

cg_image_t make_cg_image(const boost::gil::rgba8_image_t& image)
{
    cg_image_support_t* new_image =
        new cg_image_support_t(boost::gil::view(const_cast<boost::gil::rgba8_image_t&>(image)));

    return reinterpret_cast<cg_image_t>(new_image);
}

/****************************************************************************************************/

void destroy_cg_image(cg_image_t image)
{
    delete reinterpret_cast<cg_image_support_t*>(image);
}

/****************************************************************************************************/

::CGImageRef to_CGImageRef(cg_image_t image)
{
    return reinterpret_cast<cg_image_support_t*>(image)->image_ref_m.get();
}

/****************************************************************************************************/

void paint_control_with_image(::HIViewRef control, const boost::gil::rgba8_image_t& image)
{
    typedef adobe::auto_resource< adobe::cg_image_t > auto_cg_image_t;

    assert(control);

    ::WindowRef     our_window(::GetControlOwner(control));
    ::CGrafPtr      window_port(::GetWindowPort(our_window));
    ::CGContextRef  context(0);
    ::Rect          window_bounds = { 0 };
    ::Rect          bounds = { 0 };
    auto_cg_image_t auto_image(adobe::make_cg_image(image));
    ::CGImageRef    image_ref(adobe::to_CGImageRef(auto_image.get()));

    ::GetWindowBounds(our_window, kWindowContentRgn, &window_bounds);

    get_control_bounds(control, bounds, true);

    ADOBE_REQUIRE_STATUS(::QDBeginCGContext(window_port, &context));

    ::CGContextTranslateCTM(context, 0.0, static_cast<float>(window_bounds.bottom - window_bounds.top - 1)); 
    ::CGContextScaleCTM(context, 1.0, -1.0);
    ::CGContextSetLineWidth(context, 1);
    ::CGContextSetShouldAntialias(context, false);

    float x(bounds.left);
    float y(bounds.top - 1);
    float width(bounds.right - x);
    float height(bounds.bottom - y - 1);

    ::CGContextDrawImage(context, ::CGRectMake(x, y, width, height), image_ref);

    // Final actions
    ::CGContextSynchronize(context);

    ADOBE_REQUIRE_STATUS(::QDEndCGContext(window_port, &context));
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
