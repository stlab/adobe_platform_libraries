/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_image.hpp>

#include <adobe/algorithm/clamp.hpp>
#include <adobe/future/macintosh_graphic_utils.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const long fixed_width = 250;
const long fixed_height = fixed_width;

/****************************************************************************************************/

pascal void draw_label_image(ControlRef control, SInt16)
try
{
    typedef adobe::auto_resource< adobe::cg_image_t > auto_cg_image_t;

    assert(control);

    adobe::image_t& label(*reinterpret_cast<adobe::image_t*>(::GetControlReference(control)));
    ::WindowRef     our_window(::GetControlOwner(control));
    ::CGrafPtr      window_port(::GetWindowPort(our_window));
    ::CGContextRef  context(0);
    ::Rect          window_bounds = { 0 };
    ::Rect          bounds = { 0 };
    auto_cg_image_t auto_image(adobe::make_cg_image(label.image_m));
    ::CGImageRef    image_ref(adobe::to_CGImageRef(auto_image.get()));
    adobe::auto_resource< ::CGImageRef > image_crop;

    ::GetWindowBounds(our_window, kWindowContentRgn, &window_bounds);

    adobe::implementation::get_bounds(control, bounds, true);

    ADOBE_REQUIRE_STATUS(::QDBeginCGContext(window_port, &context));

    ::CGContextTranslateCTM(context, 0.0, static_cast<float>(window_bounds.bottom - window_bounds.top - 1)); 
    ::CGContextScaleCTM(context, 1.0, -1.0);
    ::CGContextSetLineWidth(context, 1);
    ::CGContextSetShouldAntialias(context, false);

    float x(bounds.left);
    float y(bounds.top - 1);
    float width(bounds.right - x);
    float height(bounds.bottom - y - 1);

    if (label.callback_m)
    {
        ::CGRect crop_bounds(::CGRectMake(label.origin_m.first,
                                          label.origin_m.second,
                                          std::min<std::size_t>(fixed_width, label.image_m.width()),
                                          std::min<std::size_t>(fixed_height, label.image_m.height())));

        image_crop.reset(::CGImageCreateWithImageInRect(image_ref, crop_bounds));

        ::CGContextDrawImage(context,
                             ::CGRectMake(x + std::max<long>((long(fixed_width) - long(label.image_m.width())) / 2, 0),
                                          y + std::max<long>((long(fixed_height) - long(label.image_m.height())) / 2, 0),
                                          std::min<std::size_t>(fixed_width, label.image_m.width()),
                                          std::min<std::size_t>(fixed_height, label.image_m.height())),
                             image_crop.get());
        ::CGContextStrokeRect(context, ::CGRectMake(x, y, width, height));
    }
    else
    {
        ::CGContextDrawImage(context, ::CGRectMake(x, y, width, height), image_ref);
    }

    // Final actions
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

pascal ControlPartCode hit_test_toggle(ControlRef /*control*/, Point /*where*/)
{
    return 1;
}

/****************************************************************************************************/

ControlUserPaneDrawUPP draw_handler_upp()
{
    static adobe::auto_resource< ::ControlUserPaneDrawUPP > draw_handler_s(::NewControlUserPaneDrawUPP(draw_label_image));

    return draw_handler_s.get();
}

/****************************************************************************************************/

ControlUserPaneHitTestUPP hit_test_handler_upp()
{
    static adobe::auto_resource< ::ControlUserPaneHitTestUPP > hit_test_handler_s(::NewControlUserPaneHitTestUPP(hit_test_toggle));

    return hit_test_handler_s.get();
}

/****************************************************************************************************/

void initialize(adobe::image_t& value, ::HIViewRef /*parent*/)
{
    ::Rect          bounds = { 0, 0, static_cast<short>(value.image_m.width()), static_cast<short>(value.image_m.height()) };
    ::ControlRef    tmp_control;

    ::ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(0, &bounds, 0, &tmp_control));

    value.control_m.reset(tmp_control);

    ::SetControlReference(value.control_m.get(), reinterpret_cast< ::SInt32 >(&value));

    adobe::implementation::set_widget_data(value.control_m.get(), kControlEntireControl,
                                           kControlUserPaneDrawProcTag, draw_handler_upp());
    adobe::implementation::set_widget_data(value.control_m.get(), kControlEntireControl,
                                           kControlUserPaneHitTestProcTag, hit_test_handler_upp());

    ::HIViewTrackingAreaRef dummy(0);

    ::HIViewNewTrackingArea(tmp_control, 0, 0, &dummy);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(image_t, "<xstr id='metric:image'/>");

/****************************************************************************************************/

image_t::image_t(const view_model_type& image) :
    image_m(image),
    enabled_m(false),
    handler_m(*this)
{
    metadata_m.insert(dictionary_t::value_type("delta_x"_name, any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type("delta_y"_name, any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type("dragging"_name, any_regular_t(false)));
    metadata_m.insert(dictionary_t::value_type("x"_name, any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type("y"_name, any_regular_t(0)));
}

/****************************************************************************************************/

void image_t::display(const view_model_type& value)
{
    image_m = value;

    if (image_m.width() <= fixed_width)
        origin_m.first = 0;
    else
        origin_m.first = (image_m.width() - fixed_width) / 2;

    if (image_m.height() <= fixed_height)
        origin_m.second = 0;
    else
        origin_m.second = (image_m.height() - fixed_height) / 2;

    ::HIViewSetNeedsDisplay(control_m.get(), true);
}

/****************************************************************************************************/

void image_t::enable(bool make_enabled)
{
    enabled_m = make_enabled;
}

/****************************************************************************************************/

void image_t::monitor(const setter_proc_type& proc)
{
    callback_m = proc;

    handler_m.handler_m.install(control_m.get());
}

/****************************************************************************************************/

void image_t::measure(extents_t& result)
{
    if (callback_m)
        result.horizontal().length_m = fixed_width; // REVISIT (fbrereto) : Fixed value
    else
        result.horizontal().length_m = (long)image_m.width();
}

/****************************************************************************************************/

void image_t::measure_vertical(extents_t& result, const place_data_t& placed_horizontal)
{
    if (callback_m)
        result.vertical().length_m = fixed_height; // REVISIT (fbrereto) : Fixed value
    else
    {
        // NOTE (fbrereto) : We calculate and use the aspect ratio here to
        //                   maintain a proper initial height and width in
        //                   the case when the image grew based on how it
        //                   is being laid out.

        float aspect_ratio(image_m.height() / static_cast<float>(image_m.width()));

        result.vertical().length_m = static_cast<long>(placed_horizontal.horizontal().length_m * aspect_ratio);
    }
}

/****************************************************************************************************/

void image_t::place(const place_data_t& place_data)
{
    implementation::set_bounds(*this, place_data);

    if (callback_m)
    {
        dictionary_t old_metadata(metadata_m);

        double width(callback_m ? std::min<long>(fixed_width, image_m.width()) : image_m.width());
        double height(callback_m ? std::min<long>(fixed_height, image_m.height()) : image_m.height());
    
        metadata_m.insert(dictionary_t::value_type("width"_name, any_regular_t(width)));
        metadata_m.insert(dictionary_t::value_type("height"_name, any_regular_t(height)));

        if (old_metadata != metadata_m)
            callback_m(metadata_m);
    }
}

/****************************************************************************************************/

::OSStatus image_t::handle_event(::EventHandlerCallRef /*next*/, ::EventRef event)
{
    if (::GetEventClass(event) != kEventClassControl &&
        ::GetEventKind(event) != kEventControlClick &&
        callback_m)
        return eventNotHandledErr;

    ::Point               last_point = { 0 };
    ::MouseTrackingResult track_result(0);

    ::TrackMouseLocation(0, &last_point, &track_result);

    while (track_result != kMouseTrackingMouseUp)
    {
        ::Point  cur_point = { 0 };
        ::UInt32 os_modifiers(0);

        ::TrackMouseLocationWithOptions(0, /*inPort*/
                                        0, /*inOptions*/
                                        .1, /*inTimeout (in seconds)*/
                                        &cur_point,
                                        &os_modifiers,
                                        &track_result);

        //modifiers_t modifiers(implementation::convert_modifiers(os_modifiers));

        if (track_result == kMouseTrackingMouseUp)
            break;

        if (last_point.v != cur_point.v ||
            last_point.h != cur_point.h)
        {
            double x(0);
            double y(0);
            double delta_x(last_point.h - cur_point.h);
            double delta_y(last_point.v - cur_point.v);

            get_value(metadata_m, "x"_name, x);
            get_value(metadata_m, "y"_name, y);

            if (image_m.width() < fixed_width)
                x = 0;
            else
                x = adobe::clamp<long>(x + delta_x, 0, image_m.width() - fixed_width);

            if (image_m.height() < fixed_height)
                y = 0;
            else
                y = adobe::clamp<long>(y + delta_y, 0, image_m.height() - fixed_height);

            metadata_m.insert(dictionary_t::value_type("delta_x"_name, any_regular_t(delta_x)));
            metadata_m.insert(dictionary_t::value_type("delta_y"_name, any_regular_t(delta_y)));
            metadata_m.insert(dictionary_t::value_type("dragging"_name, any_regular_t(true)));
            metadata_m.insert(dictionary_t::value_type("x"_name, any_regular_t(x)));
            metadata_m.insert(dictionary_t::value_type("y"_name, any_regular_t(y)));

            callback_m(metadata_m);

            origin_m.first += delta_x;
            origin_m.second -= delta_y; // flipped because CoreGraphics' y-axis is flipped.

            origin_m.first = adobe::clamp<long>(origin_m.first, 0, std::max<long>(0, image_m.width() - fixed_width));
            origin_m.second = adobe::clamp<long>(origin_m.second, 0, std::max<long>(0, image_m.height() - fixed_height));

            ::HIViewSetNeedsDisplay(control_m.get(), true);
        }

        last_point = cur_point;
    }

    metadata_m.insert(dictionary_t::value_type("delta_x"_name, any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type("delta_y"_name, any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type("dragging"_name, any_regular_t(false)));

    callback_m(metadata_m);

    return noErr;
}

/*************************************************************************************************/

template <>
platform_display_type insert<image_t>(display_t&             display,
                                      platform_display_type& parent,
                                      image_t&               element)
{
    initialize(element, parent);

    return display.insert(parent, element.control_m.get());
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
