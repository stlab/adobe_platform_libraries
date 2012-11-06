/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/algorithm/copy.hpp>

#include <adobe/future/widgets/headers/platform_reveal.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

/*!
    (Intended for internal use only) Handles initialization of the widget
*/

void initialize(adobe::reveal_t& control,
                ::HIViewRef      parent)
{
    static const ::Rect bounds_s = { 0, 0, 1024, 1024 }; 
    bool         draw_label(control.name_m.empty() == false);
    ::ControlRef ctrl(0);

    ADOBE_REQUIRE_STATUS(::CreateDisclosureTriangleControl(::GetControlOwner(parent),
                                                           &bounds_s,
                                                           kControlDisclosureTrianglePointDefault,
                                                           adobe::explicit_cast<adobe::auto_cfstring_t>(control.name_m).get(),
                                                           0,          /*inInitialValue*/
                                                           draw_label, /*inDrawTitle*/
                                                           false,      /*inAutoToggles*/
                                                           &ctrl));

    control.control_m.reset(ctrl);

    adobe::implementation::set_theme(control, control.theme_m);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(ctrl, control.alt_text_m);
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(reveal_t, "<xstr id='metric:reveal'/>");

/*************************************************************************************************/

::OSStatus reveal_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                              ::EventRef            /*event*/)
{
    OSStatus result(noErr);

    if (setter_m.empty())
        return eventNotHandledErr;

    setter_m(widget_m.last_m == widget_m.show_value_m ? any_regular_t(empty_t()) : widget_m.show_value_m);

    return result;
}

/*************************************************************************************************/
    
reveal_t::reveal_t(const std::string&   name,
                   const any_regular_t& show_value,
                   theme_t              theme,
                   const std::string&   alt_text) :
    control_m(0),
    theme_m(theme),
    name_m(name),
    hit_handler_m(*this),
    show_value_m(show_value),
    alt_text_m(alt_text)
{ }

/*************************************************************************************************/

void reveal_t::measure(extents_t& result)
{
    assert(control_m);

    result = implementation::measure(*this);
    
    #if 0

    // REVISIT (fbrereto): Apparently drawing the label of a reveal widget isn't supported in 10.4.7.

    std::string name(implementation::get_name(*this));

    if (name.empty())
        return;

    extents_t label_extra(implementation::measure_theme_text(name, theme_m));

    result.width() += 4 /*gap*/ + label_extra.width();
    result.height() = std::max(result.height(), label_extra.height());

    copy(label_extra.vertical().guide_set_m, std::back_inserter(result.vertical().guide_set_m));
    #endif
}

/*************************************************************************************************/

void reveal_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/*************************************************************************************************/

void reveal_t::enable(bool make_enabled)
{
    implementation::set_active(control_m, make_enabled);
}

/*************************************************************************************************/

void reveal_t::display(const any_regular_t& value)
{
    if (last_m == value)
        return;

    last_m = value;

    long show_state(last_m == show_value_m);

    ::SetControl32BitValue(control_m.get(), show_state);
}

/*************************************************************************************************/

void reveal_t::monitor(setter_type proc)
{
    assert(control_m);

    hit_handler_m.setter_m = proc;
    hit_handler_m.handler_m.install(control_m.get());   
}

/*************************************************************************************************/

// REVISIT: MM--we need to replace the display_t mechanism with concepts/any_*/container idiom for event and drawing system.

template <>
platform_display_type insert<reveal_t>(display_t&             display,
                                              platform_display_type& parent,
                                              reveal_t&       element)
{
    initialize(element, parent);

    return display.insert(parent, element.control_m.get());
}              

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
