/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <adobe/future/widgets/headers/platform_button.hpp>

#include <adobe/future/widgets/headers/button_helper.hpp>
#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

inline void add_to_name_set(std::vector<std::string>&         name_set,
                            adobe::button_state_descriptor_t& current)
{
    name_set.push_back(current.name_m);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(button_t, "<xstr id='metric:button'/>");

/****************************************************************************************************/

::OSStatus button_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                              ::EventRef            event)
{
    assert(button_m);

    button_t& button(*button_m);
    ::UInt32  os_modifiers;

    get_event_parameter<kEventParamKeyModifiers>(event, os_modifiers);

    modifiers_t                  modifiers(implementation::convert_modifiers(os_modifiers));
    button_state_set_t::iterator state(button_modifier_state(button.state_set_m, button.modifier_mask_m, modifiers));

    if (state == button.state_set_m.end())
        state = button_default_state(button.state_set_m);

    if (!state->hit_proc_m.empty())
        state->hit_proc_m(state->value_m, state->contributing_m);

    return noErr;
}

/****************************************************************************************************/

::OSStatus button_modifier_key_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                       ::EventRef            event)
{
    assert(button_m);

    button_t& button(*button_m);
    ::UInt32  os_modifiers;

    get_event_parameter<kEventParamKeyModifiers>(event, os_modifiers);

    modifiers_t                  modifiers(implementation::convert_modifiers(os_modifiers));
    button_state_set_t::iterator state(button_modifier_state(button.state_set_m, button.modifier_mask_m, modifiers));

    if (state == button.state_set_m.end())
        state = button_default_state(button.state_set_m);

    implementation::set_name(button.control_m, state->name_m);
    implementation::set_control_alt_text(button.control_m, state->alt_text_m);

    return noErr;
}

/****************************************************************************************************/

button_t::button_t(bool                             is_default,
                   bool                             is_cancel,
                   modifiers_t                      modifier_mask,
                   const button_state_descriptor_t* first,
                   const button_state_descriptor_t* last,
                   theme_t                          theme) :
    control_m(0),
    theme_m(theme),
    state_set_m(first, last),
    hit_handler_m(this),
    mod_key_handler_m(this),
    modifier_mask_m(modifier_mask),
    is_default_m(is_default),
    is_cancel_m(is_cancel),
    enabled_m(true)
{ }

/****************************************************************************************************/

void button_t::measure(extents_t& result)
{
    assert(control_m);

    std::vector<std::string> name_set;

    for_each(state_set_m, boost::bind(&add_to_name_set, boost::ref(name_set), _1));

    result = implementation::widget_best_bounds(*this, &name_set[0], &name_set[0] + name_set.size());

    result.horizontal().length_m =
        std::max<long>(70 /* REVISIT (fbrereto) : fixed value */, result.horizontal().length_m );
}

/****************************************************************************************************/

void button_t::place(const place_data_t& place_data)
{
    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void button_t::enable(bool make_enabled)
{
    enabled_m = make_enabled;

    if (control_m) implementation::set_active(control_m, make_enabled);
}

/****************************************************************************************************/

void button_t::set(modifiers_t modifiers, const any_regular_t& value)
{
    button_state_set_t::iterator state(button_modifier_state(state_set_m, modifier_mask_m, modifiers));

    if (state == state_set_m.end())
        state = button_default_state(state_set_m);

    if (state->value_m != value)
        state->value_m = value;
}

/****************************************************************************************************/

void button_t::set_contributing(modifiers_t modifiers, const dictionary_t& value)
{
    button_state_set_t::iterator state(button_modifier_state(state_set_m, modifier_mask_m, modifiers));

    if (state == state_set_m.end())
        state = button_default_state(state_set_m);

    state->contributing_m = value;
}

/****************************************************************************************************/

bool button_t::handle_key(key_type key, bool pressed, modifiers_t modifiers)
{
    if (pressed == false)
        return false;

    button_state_set_t::iterator state(button_modifier_state(state_set_m, modifier_mask_m, modifiers));

    if (state == state_set_m.end())
        state = button_default_state(state_set_m);

    if (state->hit_proc_m.empty() || enabled_m == false)
        return false;

    if (key == 0x0d && is_default_m) // return
    {
        state->hit_proc_m(state->value_m, state->contributing_m);
    }
    else if (key == 0x1b && is_cancel_m) // escape
    {
        state->hit_proc_m(state->value_m, state->contributing_m);
    }
    else
    {
        return false;
    }

    return true;
}

/****************************************************************************************************/

template <>
platform_display_type insert<button_t>(display_t&             display,
                                       platform_display_type& parent,
                                       button_t&              element)
{
    static const ::Rect bounds_s = { 0, 0, 1024, 1024 };

    assert(element.control_m == false);

    button_state_set_t::iterator state(button_default_state(element.state_set_m));
    ::HIViewRef                  parent_ref(parent);
    ::WindowRef                  window(::GetControlOwner(parent_ref));

    ADOBE_REQUIRE_STATUS(::CreatePushButtonControl(window,
                                                   &bounds_s,
                                                   explicit_cast<auto_cfstring_t>(state->name_m).get(),
                                                   &element.control_m));

    implementation::set_theme(element.control_m, element.theme_m);
    implementation::set_active(element.control_m, element.enabled_m);

    if (state->alt_text_m.empty() == false)
        implementation::set_control_alt_text(element.control_m, state->alt_text_m);

    element.hit_handler_m.handler_m.install(element.control_m);

    implementation::set_widget_data(element.control_m,
                                    kControlEntireControl,
                                    kControlPushButtonDefaultTag,
                                    static_cast< ::Boolean >(element.is_default_m));

    implementation::set_widget_data(element.control_m,
                                    kControlEntireControl,
                                    kControlPushButtonCancelTag,
                                    static_cast< ::Boolean >(element.is_cancel_m));

    assert(element.mod_key_handler_m.handler_m.is_installed() == false);

    if (element.state_set_m.size() > 1)
        element.mod_key_handler_m.handler_m.install(window);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
