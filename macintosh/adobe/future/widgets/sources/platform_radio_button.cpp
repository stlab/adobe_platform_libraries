/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_radio_button.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(radio_button_t, "<xstr id='metric:radio_button'/>");

/****************************************************************************************************/

::OSStatus radio_button_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                    ::EventRef            /*event*/)
{
    if (setter_m.empty())
        return eventNotHandledErr;

    std::size_t value(static_cast<std::size_t>(implementation::get_value(widget_m.control_m)));

    if (value == kControlRadioButtonCheckedValue && widget_m.last_m != widget_m.set_value_m)
    {
        widget_m.last_m = widget_m.set_value_m;

        setter_m(widget_m.set_value_m);
    }

    return noErr;
}

/****************************************************************************************************/

radio_button_t::radio_button_t(const std::string&          name,
                               const std::string&          alt_text,
                               const any_regular_t& value,
                               theme_t              theme) :
    control_m(0),
    name_m(name),
    alt_text_m(alt_text),
    set_value_m(value),
    theme_m(theme),
    hit_handler_m(*this)
{ }

/****************************************************************************************************/

void radio_button_t::measure(extents_t& result)
{
    assert(control_m);

    result = implementation::widget_best_bounds(*this, implementation::get_name(*this));
}

/****************************************************************************************************/

void radio_button_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void radio_button_t::enable(bool make_enabled)
{
    assert(control_m);

    implementation::set_active(control_m, make_enabled);
}

/****************************************************************************************************/

void radio_button_t::display(const any_regular_t& value)
{
    if (last_m == value)
        return;

    last_m = value;

    long new_value((last_m == set_value_m) ?
                    kControlRadioButtonCheckedValue :
                    kControlRadioButtonUncheckedValue);

    ::SetControl32BitValue(control_m, new_value);
}

/****************************************************************************************************/

void radio_button_t::monitor(const radio_button_monitor_proc_t& proc)
{
    assert(control_m);

    hit_handler_m.setter_m = proc;
    hit_handler_m.handler_m.install(control_m);   
}

/****************************************************************************************************/

template <>
platform_display_type insert<radio_button_t>(display_t&             display,
                                                    platform_display_type& parent,
                                                    radio_button_t& element)
{
    static const ::Rect bounds_s = { 0, 0, 1024, 1024 };

    ::HIViewRef parent_ref(parent);

    ADOBE_REQUIRE_STATUS(::CreateRadioButtonControl(::GetControlOwner(parent_ref),
                                                    &bounds_s,
                                                    explicit_cast<auto_cfstring_t>(element.name_m).get(),
                                                    0, 1,
                                                    &element.control_m));

    implementation::set_theme(element, element.theme_m);

    if (!element.alt_text_m.empty())
        implementation::set_control_alt_text(element.control_m, element.alt_text_m);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
