/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_button.hpp>

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <adobe/future/widgets/headers/button_helper.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

LRESULT CALLBACK button_subclass_proc(HWND     window,
                                      UINT     message,
                                      WPARAM   wParam,
                                      LPARAM   lParam,
                                      UINT_PTR ptr,
                                      DWORD_PTR /* ref */)
{
    adobe::button_t& button(*reinterpret_cast<adobe::button_t*>(ptr));

    if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
        adobe::button_state_set_t::iterator state(adobe::button_modifier_state(button.state_set_m,
                                                                               button.modifier_mask_m,
                                                                               button.modifiers_m));

        if (state == button.state_set_m.end())
            state = adobe::button_default_state(button.state_set_m);

        if (!state->hit_proc_m.empty())
            state->hit_proc_m(state->value_m, state->contributing_m);
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

button_t::button_t(bool                             is_default,
                   bool                             is_cancel,
                   modifiers_t               modifier_mask,
                   const button_state_descriptor_t* first,
                   const button_state_descriptor_t* last,
                   theme_t                          theme) :
    control_m(0),
    theme_m(theme),
    state_set_m(first, last),
    modifier_mask_m(modifier_mask),
    modifiers_m(modifiers_none_s),
    is_default_m(is_default),
    is_cancel_m(is_cancel),
    enabled_m(true)
{ }

/****************************************************************************************************/

void button_t::measure(extents_t& result)
{
    result = metrics::measure(control_m, BP_PUSHBUTTON);

    button_state_set_t::iterator state(button_modifier_state(state_set_m,
                                                                           modifier_mask_m,
                                                                           modifiers_m));

    if (state == state_set_m.end())
        state = button_default_state(state_set_m);

    extents_t cur_text_extents(measure_text(state->name_m, theme_m, ::GetParent(control_m)));

    result.width() -= cur_text_extents.width();
    result.height() -= cur_text_extents.height();

    long width_additional(0);
    long height_additional(0);
    
    for (button_state_set_t::iterator iter(state_set_m.begin()), last(state_set_m.end()); iter != last; ++iter)
    {
        extents_t tmp(measure_text(iter->name_m, theme_m, ::GetParent(control_m)));

        width_additional = std::max<int>(width_additional, tmp.width());
        height_additional = std::max<int>(height_additional, tmp.height());
    }

    result.width() += width_additional;
    result.height() += height_additional;

    result.width() = std::max<int>(result.width(), 70L);
}

/****************************************************************************************************/

void button_t::place(const place_data_t& place_data)
{
    assert(control_m);
    
    implementation::set_control_bounds(control_m, place_data);
}

/****************************************************************************************************/

void button_t::enable(bool make_enabled)
{
    enabled_m = make_enabled;
    if(control_m)
        ::EnableWindow(control_m, make_enabled);
}

/****************************************************************************************************/

void button_t::set(modifiers_t modifiers, const model_type& value)
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

bool button_t::handle_key(key_type key, bool pressed, modifiers_t /* modifiers */)
{
    if (pressed == false)
        return false;

    modifiers_m = modifier_state();

    //
    // Look up the state which this modifier should trigger.
    //
    button_state_set_t::iterator state(button_modifier_state(state_set_m,
                                                                           modifier_mask_m,
                                                                           modifiers_m));

    if (state == state_set_m.end())
        state = button_default_state(state_set_m);

    //
    // Set the window text.
    //
    ::SetWindowTextW(control_m, hackery::convert_utf(state->name_m).c_str());

    //
    // Set the alt text if need be.
    //
    if (!state->alt_text_m.empty())
        implementation::set_control_alt_text(control_m, state->alt_text_m);

    if (state->hit_proc_m.empty() || enabled_m == false)
        return false;

    if (key == VK_RETURN && is_default_m) // return
    {
        state->hit_proc_m(state->value_m, state->contributing_m);
    }
    else if (key == VK_ESCAPE && is_cancel_m) // escape
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
                                                button_t&     element)
{
    HWND parent_hwnd(parent);

    assert(element.control_m == 0);

    button_state_set_t::iterator state(button_default_state(element.state_set_m));

    DWORD win_style(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_NOTIFY);

    if (element.is_default_m)
        win_style |= BS_DEFPUSHBUTTON;

    element.control_m = ::CreateWindowExW(WS_EX_COMPOSITED /*| WS_EX_TRANSPARENT*/, L"BUTTON",
                                          hackery::convert_utf(state->name_m).c_str(),
                                          win_style,
                                          0, 0, 70, 20,
                                          parent_hwnd,
                                          0,
                                          ::GetModuleHandle(NULL),
                                          NULL);

    if (element.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    set_font(element.control_m, BP_PUSHBUTTON);

    ::SetWindowSubclass(element.control_m, &button_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);

    if (!state->alt_text_m.empty())
        implementation::set_control_alt_text(element.control_m, state->alt_text_m);

    platform_display_type result(display.insert(parent, element.control_m));
    ::EnableWindow(element.control_m, element.enabled_m);
    return result;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
