/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_slider.hpp>

#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(slider_t, "<xstr id='metric:slider'/>");

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

pascal void slider_action(ControlRef control_ref, ControlPartCode)
try
{
    adobe::slider_t& control(*reinterpret_cast<adobe::slider_t*>(::GetControlReference(control_ref)));

    if (control.value_proc_m.empty())
        return;

    long new_position(adobe::implementation::get_value(control.control_m));
    double new_value(control.format_m.at(new_position).cast<double>());

    if (new_value != control.value_m)
    {
        control.value_m = new_value;

        control.value_proc_m(static_cast<adobe::slider_t::model_type>(control.value_m));
    }
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
    adobe::report_error("Unknown Error");
}

/****************************************************************************************************/

void initialize(adobe::slider_t& control, ::HIViewRef parent)
{
    assert(!control.control_m);

    static const ::Rect             hbounds = { 0, 0, 10, 100 };
    static const ::Rect             vbounds = { 0, 0, 100, 10 };
    static const ::ControlActionUPP handler(::NewControlActionUPP(slider_action));
    ::ControlSliderOrientation      orientation(kControlSliderDoesNotPoint);

    if (control.style_m == adobe::slider_points_up_s || control.style_m == adobe::slider_points_left_s)
        orientation = kControlSliderPointsUpOrLeft;
    else if (control.style_m == adobe::slider_points_down_s || control.style_m == adobe::slider_points_right_s)
        orientation = kControlSliderPointsDownOrRight;

    ADOBE_REQUIRE_STATUS(::CreateSliderControl(::GetControlOwner(parent),
                                               (control.is_vertical_m ? &vbounds : &hbounds),
                                               0, 0, 5,
                                               orientation,
                                               static_cast< ::UInt16 >(control.num_ticks_m),
                                               true,
                                               handler, &control.control_m));

    adobe::implementation::set_theme(control, control.theme_m);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);

    ::SetControl32BitMinimum(control.control_m, 0);
    ::SetControl32BitMaximum(control.control_m, control.format_m.size());
    ::SetControlReference(control.control_m, reinterpret_cast< ::SInt32 >(&control));
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

slider_t::slider_t(const std::string&          alt_text,
                   bool                        is_vertical,
                   slider_style_t              style,
                   std::size_t                 num_ticks,
                   const value_range_format_t& format,
                   theme_t                     theme) :
    control_m(0),
    alt_text_m(alt_text),
    is_vertical_m(is_vertical),
    style_m(style),
    num_ticks_m(num_ticks),
    format_m(format),
    theme_m(theme)
{ }

/****************************************************************************************************/

void slider_t::measure(extents_t& result)
{
    result = implementation::measure(*this);
}

/****************************************************************************************************/

void slider_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void slider_t::enable(bool make_enabled)
{
    assert(control_m);

    if (make_enabled)
        ::EnableControl(control_m);
    else
        ::DisableControl(control_m);
}

/****************************************************************************************************/

void slider_t::display(const model_type& value)
{
    assert(control_m);

    value_m = value;

    long new_position(format_m.find(any_regular_t(value)));

    if (new_position != last_m)
    {
        last_m = new_position;

        ::SetControl32BitValue(control_m, last_m);
    }
}

/****************************************************************************************************/

void slider_t::monitor(const setter_type& proc)
{
    value_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert(display_t& display, platform_display_type& parent, slider_t& element)
{
    ::HIViewRef parent_ref(parent);

    initialize(element, parent_ref);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
