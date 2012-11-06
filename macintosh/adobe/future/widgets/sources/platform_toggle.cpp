/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_toggle.hpp>

#include <adobe/future/macintosh_graphic_utils.hpp>
#include <adobe/future/widgets/headers/button_helper.hpp>
#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const adobe::toggle_t::image_type& current_image(::ControlRef control)
{
    assert(control);
    assert(::GetControlReference(control));

    adobe::toggle_t& toggle =
        *reinterpret_cast<adobe::toggle_t*>(::GetControlReference(control));

    if (::IsControlEnabled(control))
    {
        if (toggle.last_m == toggle.value_on_m)
            return toggle.image_on_m;
        else
            return toggle.image_off_m;
    }
    else // disabled_button
    {
        return toggle.image_disabled_m;
    }
}

/****************************************************************************************************/

pascal void draw_toggle(ControlRef control, SInt16)
try
{
    adobe::paint_control_with_image(control, current_image(control));
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
try
{
    return 1;
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

ControlUserPaneDrawUPP draw_handler_upp()
{
    static adobe::auto_resource< ::ControlUserPaneDrawUPP > draw_handler_s(::NewControlUserPaneDrawUPP(draw_toggle));

    return draw_handler_s.get();
}

/****************************************************************************************************/

ControlUserPaneHitTestUPP hit_test_handler_upp()
{
    static adobe::auto_resource< ::ControlUserPaneHitTestUPP > hit_test_handler_s(::NewControlUserPaneHitTestUPP(hit_test_toggle));

    return hit_test_handler_s.get();
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(toggle_t, "<xstr id='metric:toggle'/>");

/****************************************************************************************************/

::OSStatus toggle_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                              ::EventRef            /*event*/)
{
    assert(toggle_m);

    toggle_t& toggle(*toggle_m);

    if (!toggle.setter_proc_m)
        return noErr;

    if (toggle.last_m == toggle.value_on_m)
        toggle.setter_proc_m(any_regular_t(empty_t()));
    else
        toggle.setter_proc_m(toggle.value_on_m);

    return noErr;
}

/****************************************************************************************************/

toggle_t::toggle_t(const std::string&  alt_text,
                   const any_regular_t value_on,
                   const image_type&   image_on,
                   const image_type&   image_off,
                   const image_type&   image_disabled,
                   theme_t             theme) :
    control_m(0),
    theme_m(theme),
    hit_handler_m(this),
    alt_text_m(alt_text),
    image_on_m(image_on),
    image_off_m(image_off),
    image_disabled_m(image_disabled),
    value_on_m(value_on)
{ }

/****************************************************************************************************/

void toggle_t::measure(extents_t& result)
{
    assert(control_m);

    result = extents_t();

    const adobe::toggle_t::image_type& image(current_image(control_m));

    result.height() = image.height();
    result.width() = image.width();
}

/****************************************************************************************************/

void toggle_t::place(const place_data_t& place_data)
{
    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void toggle_t::enable(bool make_enabled)
{
    assert(control_m);

    implementation::set_active(control_m, make_enabled);

    ::HIViewSetNeedsDisplay(control_m, true);
}

/****************************************************************************************************/

void toggle_t::display(const any_regular_t& to_value)
{
    assert(control_m);

    if (last_m == to_value)
        return;

    last_m = to_value;

    ::HIViewSetNeedsDisplay(control_m, true);
}

/****************************************************************************************************/

void toggle_t::monitor(const setter_type& proc)
{
    assert(control_m);

    setter_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<toggle_t>(display_t&             display,
                                       platform_display_type& parent,
                                       toggle_t&              element)
{
    static const ::Rect bounds_s = { 0, 0, 1024, 1024 };

    assert(element.control_m == false);

    ::ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(0, &bounds_s, 0, &element.control_m));

    ::SetControlReference(element.control_m, reinterpret_cast< ::SInt32 >(&element));

    adobe::implementation::set_widget_data(element.control_m, kControlEntireControl,
                                           kControlUserPaneDrawProcTag, draw_handler_upp());

    adobe::implementation::set_widget_data(element.control_m, kControlEntireControl,
                                           kControlUserPaneHitTestProcTag, hit_test_handler_upp());

    if (element.alt_text_m.empty() == false)
        implementation::set_control_alt_text(element.control_m, element.alt_text_m);

    element.hit_handler_m.handler_m.install(element.control_m);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
