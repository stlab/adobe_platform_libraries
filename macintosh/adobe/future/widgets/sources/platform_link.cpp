/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_link.hpp>

#include <adobe/algorithm/copy.hpp>
#include <adobe/future/macintosh_graphic_utils.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <boost/gil/image.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::vector<std::pair<long, long> > point_set_t;

/****************************************************************************************************/

#if 0

void draw_line_path(::CGContextRef context, point_set_t& points)
{
    if (points.size() < 2)
        return;

    point_set_t::iterator iter(points.begin());
    point_set_t::iterator last(points.end());

    ::CGContextMoveToPoint(context, static_cast<float>(iter->first), static_cast<float>(iter->second));

    while (++iter != last)
        ::CGContextAddLineToPoint(context, static_cast<float>(iter->first), static_cast<float>(iter->second));

    ::CGContextStrokePath(context);
    ::CGContextClosePath(context);

    points.erase(points.begin(), points.end());
}

#endif

/****************************************************************************************************/

pascal void draw_link(::ControlRef control, ::SInt16)
try
{
    typedef adobe::auto_resource< adobe::cg_image_t > auto_cg_image_t;

    static const long   link_line_width(2);

    adobe::link_t&    link(*reinterpret_cast<adobe::link_t*>(::GetControlReference(control)));

    Rect               bounds = { 0 };
    WindowRef          our_window(::GetControlOwner(control));
    CGrafPtr           window_port(::GetWindowPort(our_window));
    std::size_t        num_prongs(link.prongs_m.size());
    Rect               window_bounds = { 0 };
    CGContextRef       context(0);
    point_set_t        points;
    boost::gil::rgba8_image_t gil_image;

    adobe::image_slurp("link_icon.tga", gil_image);

    auto_cg_image_t image(adobe::make_cg_image(gil_image));
    CGImageRef      link_icon(adobe::to_CGImageRef(image.get()));

    ::GetWindowBounds(our_window, kWindowContentRgn, &window_bounds);
    adobe::implementation::get_bounds(control, bounds, true);

    ADOBE_REQUIRE_STATUS(::QDBeginCGContext(window_port, &context));

    ::CGContextTranslateCTM(context, 0.0, static_cast<float>(window_bounds.bottom - window_bounds.top - 1)); 
    ::CGContextScaleCTM(context, 1.0, -1.0);
    ::CGContextSetLineWidth(context, link_line_width);
    ::CGContextSetShouldAntialias(context, false);

    ::CGContextBeginPath(context);

    // REVISIT (sparent) : If this were aligned top instead of fill then the height _should_ be correct.
    // REVISIT (sparent) : We should require up front (and default) to a count of 2.

    ::CGContextSetRGBFillColor(context, 0, 0, 0, 1);

    // REVISIT (sparent) FIXED VALUE
    bounds.right -= 6; // inset the rect a bit to create padding for the link icon

    if (::IsControlEnabled(control) && num_prongs > 1)
    {
        for (std::size_t i(0); i < num_prongs; ++i)
        {
            ::CGContextFillRect(context, ::CGRectMake(  static_cast<float>(bounds.left),
                                                        static_cast<float>(bounds.top + link.prongs_m[i] - link_line_width - 1),
                                                        static_cast<float>(bounds.right - bounds.left),
                                                        static_cast<float>(link_line_width)));
        }

        ::CGContextFillRect(context, ::CGRectMake(  static_cast<float>(bounds.right),
                                                    static_cast<float>(bounds.top + link.prongs_m[0] - link_line_width - 1),
                                                    static_cast<float>(link_line_width),
                                                    static_cast<float>(link.prongs_m[num_prongs - 1] - link.prongs_m[0] + link_line_width)));

        bounds.right += 2; // reset inset for the link icon

        long    center =    bounds.top +
                            link.prongs_m[0] - link_line_width +
                            (link.prongs_m[num_prongs - 1] - link.prongs_m[0]) / 2;

        ::CGContextDrawImage(context, ::CGRectMake(bounds.right - 5.0f, center - 8.0f, 9.0f, 16.0f), link_icon);
    }

    ::CGContextSynchronize(context);

    ADOBE_REQUIRE_STATUS(::QDEndCGContext(window_port, &context));
}
catch (const adobe::stream_error_t& err)
{
    adobe::report_error(adobe::format_stream_error(err));
}
catch (const std::exception& err)
{
    adobe::report_error(std::string("Exception: ") + err.what());
}
catch (...)
{
    adobe::report_error("Exception: Unknown");
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(link_t, "<xstr id='metric:link'/>");

/****************************************************************************************************/

::OSStatus link_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                            ::EventRef            event)
{
    if (setter_m.empty())
        return eventNotHandledErr;

    ::UInt32 modifiers;

    get_event_parameter<kEventParamKeyModifiers>(event, modifiers);

    setter_m(widget_m.off_value_m);

    return noErr;
}

/****************************************************************************************************/

link_t::link_t(const std::string&   alt_text,
               const any_regular_t& on_value,
               const any_regular_t& off_value,
               long                 count,
               theme_t              theme) :
    control_m(0),
    alt_text_m(alt_text),
    on_value_m(on_value),
    off_value_m(off_value),
    count_m(count),
    theme_m(theme),
    hit_handler_m(*this)
{ }

/****************************************************************************************************/

void link_t::measure(extents_t& result)
{
    assert(control_m);

    result.horizontal().length_m   = 15;
    result.vertical().length_m     = 5;

    for (long i(0); i < count_m; ++i)
        result.vertical().guide_set_m.push_back(2);

    result = implementation::apply_fudges(*this, result);
}

/****************************************************************************************************/

void link_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);

    prongs_m.erase(prongs_m.begin(), prongs_m.end());

    copy(place_data.vertical().guide_set_m, std::back_inserter(prongs_m));
}

/****************************************************************************************************/

void link_t::enable(bool make_enabled)
{
    assert(control_m);

    implementation::set_active(control_m, make_enabled);
    implementation::set_visible(control_m, value_m == on_value_m && make_enabled);
}

/****************************************************************************************************/

void link_t::display(const any_regular_t& value)
{
    assert(control_m);

    value_m = value;

    implementation::set_visible(control_m, value_m == on_value_m && ::IsControlEnabled(control_m));
}

/****************************************************************************************************/

void link_t::monitor(const setter_type& proc)
{
    assert(control_m);

    hit_handler_m.setter_m = proc;
    hit_handler_m.handler_m.install(control_m);
}

/****************************************************************************************************/

bool operator==(const link_t& /*x*/, const link_t& /*y*/)
{ return true; }

/****************************************************************************************************/

template <>
platform_display_type insert<link_t>(display_t&             display,
                                     platform_display_type& parent,
                                     link_t&                element)
{
    static const ::Rect             bounds_s = { 0, 0, 10, 10 };
    static ::ControlUserPaneDrawUPP draw_handler(::NewControlUserPaneDrawUPP(draw_link));

    ::HIViewRef parent_ref(parent);

    assert(!element.control_m);

    ::ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(::GetControlOwner(parent_ref), &bounds_s, 0, &element.control_m));

    ::SetControlReference(element.control_m, reinterpret_cast< ::SInt32 >(&element));

    implementation::set_widget_data(element.control_m, kControlEntireControl, kControlUserPaneDrawProcTag, draw_handler);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
