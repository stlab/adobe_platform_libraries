/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_checkbox.hpp>

#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/display.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

LRESULT CALLBACK checkbox_subclass_proc(HWND     window,
                                        UINT     message,
                                        WPARAM   wParam,
                                        LPARAM   lParam,
                                        UINT_PTR ptr,
                                        DWORD_PTR /* ref */)
{
    adobe::checkbox_t& checkbox(*reinterpret_cast<adobe::checkbox_t*>(ptr));

    if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
        if (checkbox.hit_proc_m.empty())
            return 0;

        adobe::any_regular_t new_value(checkbox.true_value_m);

        if (checkbox.current_value_m == checkbox.true_value_m)
            new_value = checkbox.false_value_m;

        checkbox.hit_proc_m(new_value);

        return 0;
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

void initialize(adobe::checkbox_t& control, HWND parent)
{
    assert(control.control_m == 0);

    control.control_m = ::CreateWindowExW(WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"BUTTON",
                                          ::hackery::convert_utf(control.name_m).c_str(),
                                          WS_CHILD | WS_VISIBLE | BS_3STATE | WS_TABSTOP | BS_NOTIFY,
                                          0, 0, 100, 20,
                                          parent,
                                          NULL,
                                          ::GetModuleHandle(NULL),
                                          NULL);

    if (control.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    ::SetWindowSubclass(control.control_m, &checkbox_subclass_proc, reinterpret_cast<UINT_PTR>(&control), 0);

    adobe::set_font(control.control_m, BP_CHECKBOX);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

checkbox_t::checkbox_t( const std::string&			name,
                        const any_regular_t& true_value,
                        const any_regular_t& false_value,
                        theme_t						theme,
                        const std::string&			alt_text) :
    control_m(0),
    theme_m(theme),
    true_value_m(true_value),
    false_value_m(false_value),
    name_m(name),
    alt_text_m(alt_text)
{ }

/****************************************************************************************************/

void checkbox_t::measure(extents_t& result)
{
    result = metrics::measure(control_m, BP_CHECKBOX);

    //
    // Get the text margins, and factor those into the bounds.
    //
    RECT margins = {0, 0, 0, 0};

    metrics::set_window(control_m);

    if (metrics::get_button_text_margins(BP_CHECKBOX, margins))
    {
        //
        // Add the width margins in. The height margins aren't important because
        // the widget is already large enough to contain big text (as calculated
        // by calculate_best_bounds).
        //
        result.width() += margins.left + margins.right;
    }
}

/****************************************************************************************************/

void checkbox_t::place(const place_data_t& place_data)
{
    assert(control_m);
    
    implementation::set_control_bounds(control_m, place_data);
}

/****************************************************************************************************/

void checkbox_t::enable(bool make_enabled)
{
    assert(control_m);
    
    ::EnableWindow(control_m, make_enabled);
}

/****************************************************************************************************/

void checkbox_t::display(const any_regular_t& new_value)
{
    assert(control_m);

    if (current_value_m == new_value) return;

    current_value_m = new_value;

    WPARAM state(BST_INDETERMINATE);

    if (current_value_m == true_value_m)
        state = BST_CHECKED;
    else if (current_value_m == false_value_m)
        state = BST_UNCHECKED;

    ::SendMessage(control_m, BM_SETCHECK, state, 0);
}

/****************************************************************************************************/

void checkbox_t::monitor(const setter_type& proc)
{
    assert(control_m);

    hit_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<checkbox_t>(display_t&             display,
                                                platform_display_type& parent,
                                                checkbox_t&     element)
{
    HWND parent_hwnd(parent);

    initialize(element, parent_hwnd);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
