/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_group.hpp>

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/display.hpp>
// #include <adobe/future/windows_cast.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

LRESULT CALLBACK group_subclass_proc(HWND     window,
                                     UINT     message,
                                     WPARAM   wParam,
                                     LPARAM   lParam,
                                     UINT_PTR /* ptr */,
                                     DWORD_PTR /* ref */)
{
    // Why does group_t have an empty set_theme and a funny event handler?
    // There seems to be a bug in Win32 with visual styles enabled where a
    // group box nested inside another group box has the wrong font if it
    // has ever been sent a WM_SETFONT message; at least, if it has been
    // sent a WM_SETFONT then it ignores the font selected into the DC in
    // the WM_CTLCOLORSTATIC handler.
    //
    // So: we don't send a WM_SETFONT by not implementing a proper set_theme.
    // We implement a WM_CTLCOLORSTATIC that selects a suitable font into the
    // group's HDC.
    //
    // Note that we probably *should* get the font using the metrics interface
    // rather than using the DEFAULT_GUI_FONT (as control_t::set_theme does).
    //

#if 0
    adobe::group_t& control(*reinterpret_cast<adobe::group_t*>(ptr));

    if (message == WM_CTLCOLORSTATIC ||
        message == WM_CTLCOLORBTN ||
        message == WM_CTLCOLOREDIT ||
        message == WM_CTLCOLORLISTBOX ||
        message == WM_CTLCOLORSCROLLBAR)
    {
        LOGFONTW log_font = { 0 };

        adobe::metrics::set_window(control.control_m);

        if (adobe::metrics::get_font(BP_GROUPBOX, log_font) == false)
            throw std::runtime_error("could not get the right font");

        HFONT font(::CreateFontIndirectW(&log_font));

        ::SelectObject(hackery::cast<HDC>(wParam), font);
        adobe::metrics::draw_parent_background(hackery::cast<HWND>(lParam), hackery::cast<HDC>(wParam));
    }
    else
#endif
    {
        LRESULT forward_result(0);

        if (adobe::forward_message(message, wParam, lParam, forward_result))
            return forward_result;
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

group_t::group_t(const std::string& name,
                 const std::string& alt_text,
                 theme_t     theme) :
    control_m(0),
    name_m(name),
    alt_text_m(alt_text),
    theme_m(theme)
{ }

/****************************************************************************************************/

void group_t::measure(extents_t& result)
{
    assert(control_m);

    if (name_m.empty())
    {
        result.height() = 15;
        result.width() = 15;

        return;
    }

    // REVISIT (fbrereto) : A lot of static metrics values added here

    result = measure_text(name_m, theme_m, ::GetParent(control_m));

    result.width() += 15;

    result.vertical().frame_m.first = result.height() + 7;

    result.height() = 5;
}

/****************************************************************************************************/

void group_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_control_bounds(control_m, place_data);
}

/****************************************************************************************************/

template <>
platform_display_type insert<group_t>(display_t&             display,
                                             platform_display_type& parent,
                                             group_t&        element)
{
    HWND parent_hwnd(parent);

    element.control_m = ::CreateWindowExW(WS_EX_CONTROLPARENT | WS_EX_COMPOSITED | WS_EX_TRANSPARENT,
                                          L"BUTTON",
                                          hackery::convert_utf(element.name_m).c_str(),
                                          WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                          0, 0, 10, 10,
                                          parent_hwnd,
                                          0,
                                          ::GetModuleHandle(NULL),
                                          NULL);

    if (element.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    set_font(element.control_m, BP_GROUPBOX);

    ::SetWindowSubclass(element.control_m, &group_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
