/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_checkbox.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

/*!
    (Intended for internal use only) Handles initialization of the widget
*/

void initialize(adobe::checkbox_t& control, ::HIViewRef parent)
{
    static const ::Rect bounds_s = { 0, 0, 1024, 1024 };

    ADOBE_REQUIRE_STATUS(::CreateCheckBoxControl(::GetControlOwner(parent),
                                                 &bounds_s,
                                                 adobe::explicit_cast<adobe::auto_cfstring_t>(control.name_m).get(),
                                                 0, 1,
                                                 &control.control_m));

    adobe::implementation::set_theme(control, control.theme_m);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(checkbox_t, "<xstr id='metric:checkbox'/>");

/****************************************************************************************************/

::OSStatus checkbox_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                ::EventRef            /*event*/)
{
    assert (!setter_m.empty());

    static const std::size_t checked_k(static_cast<std::size_t>(kControlCheckBoxCheckedValue));
    static const std::size_t unchecked_k(static_cast<std::size_t>(kControlCheckBoxUncheckedValue));

    std::size_t value(static_cast<std::size_t>(implementation::get_value(widget_m.control_m)));

    if (value == checked_k)
        setter_m(widget_m.true_value_m);
    else if (value == unchecked_k)
        setter_m(widget_m.false_value_m);
    else // unknown state -- how did you get here?
        assert(false);

    return noErr;
}

/****************************************************************************************************/

checkbox_t::checkbox_t(const std::string&    name,
                       const any_regular_t&  true_value,
                       const any_regular_t&  false_value,
                       theme_t               theme,
                       const std::string&    alt_text) :
    control_m(0),
    theme_m(theme),
    hit_handler_m(*this),
    true_value_m(true_value),
    false_value_m(false_value),
    alt_text_m(alt_text),
    name_m(name)
{ }

/****************************************************************************************************/

void checkbox_t::measure(extents_t& result)
{
    assert(control_m);

    result = implementation::widget_best_bounds(*this, implementation::get_name(*this));
}

/****************************************************************************************************/

void checkbox_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void checkbox_t::enable(bool make_enabled)
{
    assert(control_m);

    implementation::set_active(control_m, make_enabled);
}

/****************************************************************************************************/

void checkbox_t::display(const any_regular_t& value)
{
    long check_state((value == true_value_m) ?
                     kControlCheckBoxCheckedValue :
                     (value == false_value_m) ?
                        kControlCheckBoxUncheckedValue :
                        kControlCheckBoxMixedValue);

    ::SetControl32BitValue(control_m, check_state);
}

/****************************************************************************************************/

void checkbox_t::monitor(setter_type proc)
{
    assert(control_m);

    hit_handler_m.setter_m = proc;
    hit_handler_m.handler_m.install(control_m);   
}

/****************************************************************************************************/

bool operator==(const checkbox_t& /*x*/, const checkbox_t& /*y*/)
{ return true; }

/****************************************************************************************************/

template <>
platform_display_type insert<checkbox_t>(display_t&             display,
                                         platform_display_type& parent,
                                         checkbox_t&            element)
{
    ::HIViewRef parent_ref(parent);

    initialize(element, parent_ref);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
