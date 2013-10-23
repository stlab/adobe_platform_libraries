/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_window.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/keyboard.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

std::pair<long, long> get_window_client_offsets(HWND window)
{
    assert(window);

    RECT    window_rect;
    RECT    client_rect;

    ::GetWindowRect(window, &window_rect);
    ::GetClientRect(window, &client_rect);

    long extra_width = (window_rect.right - window_rect.left) - (client_rect.right - client_rect.left);
    long extra_height = (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);

    return std::make_pair(extra_width, extra_height);
}

/****************************************************************************************************/

//
/// This function is the main event handler for ui-core on Win32. It delegates
/// most of the events it recieves to individual control instances, via the
/// event_dispatch object.
///
/// \param  window  the window associated with the event.
/// \param  message the Windows event type.
/// \param  wParam  the pointer parameter.
/// \param  lParam  the integer parameter.
///
/// \return zero, or whatever the default window handler returns.
//
LRESULT CALLBACK window_event_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_SIZING)
    {
        adobe::window_t& imp(*reinterpret_cast<adobe::window_t*>(adobe::get_user_reference(window)));
        RECT&            bounds(*hackery::cast<RECT*>(lParam));

        if (imp.debounce_m == false && imp.resize_proc_m.empty() == false)
        {
            imp.debounce_m = true;

            std::pair<long, long>   extra(get_window_client_offsets(window));
            long                    width(bounds.right - bounds.left - extra.first);
            long                    height(bounds.bottom - bounds.top - extra.second);

            if (height < imp.min_size_m.y_m)
            {
                height = imp.min_size_m.y_m;
                bounds.bottom = bounds.top + imp.min_size_m.y_m + extra.second;
            }

            if (width < imp.min_size_m.x_m)
            {
                width = imp.min_size_m.x_m;
                bounds.right = bounds.left + imp.min_size_m.x_m + extra.first;
            }

            if (adobe::width(imp.place_data_m) != width || adobe::height(imp.place_data_m) != height)
            {
                imp.resize_proc_m(width, height);

                adobe::width(imp.place_data_m) = width;
                adobe::height(imp.place_data_m) = height;
            }

            imp.debounce_m = false;
        }
    }
    else if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN ||
             message == WM_KEYUP   || message == WM_SYSKEYUP)
    {
        if (adobe::keyboard_t::get().dispatch(adobe::key_type(wParam),
                                              message == WM_KEYDOWN || message == WM_SYSKEYDOWN,
                                              adobe::modifier_state(),
                                              adobe::any_regular_t(window)))
            return 0;
    }
    else
    {
        //
        // See if we can forward the message and handle it that way.
        //
        LRESULT forward_result(0);
        if (adobe::forward_message(message, wParam, lParam, forward_result))
            return forward_result;
    }
    //
    // Pass it to the default window procedure.
    //
    return ::DefWindowProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

window_t::window_t(const std::string&  name,
                   window_style_t      style,
                   window_attributes_t attributes,
                   window_modality_t   modality,
                   theme_t             theme) :
    window_m(0),
    name_m(name),
    style_m(style),
    attributes_m(attributes),
    modality_m(modality),
    theme_m(theme),
    debounce_m(false),
    placed_once_m(false)
{
    static bool inited(false);

    if (!inited)
    {
        inited = true;

        // REVISIT (fbrereto) : init_once all this stuff
        INITCOMMONCONTROLSEX control_info = { sizeof(INITCOMMONCONTROLSEX), 0x0000FFFF };
        ::InitCommonControlsEx(&control_info);

        WNDCLASSW wc;

        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &window_event_proc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = ::GetModuleHandle(NULL);
        wc.hIcon = NULL; // LoadIcon(wc.hInstance, MAKEINTRESOURCE(RES_APP_ICON));
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"eve_dialog";

        RegisterClassW(&wc);
    }
}

/****************************************************************************************************/

window_t::~window_t()
{
    if (window_m)
        ::DestroyWindow(window_m);

    window_m = 0;
}

/****************************************************************************************************/

void window_t::measure(extents_t& result)
{
    assert(window_m);

    if (name_m.empty())
    {
        result.height() = 15;
        result.width() = 15;

        return;
    }

    // REVISIT (fbrereto) : A lot of static metrics values added here

    result = measure_text(name_m, theme_m, window_m);

    result.width() = static_cast<long>(result.width() * 1.5);
}

/****************************************************************************************************/

void window_t::place(const place_data_t& place_data)
{
    assert(window_m);

    if (placed_once_m)
    {
        set_size(point_2d_t(width(place_data), height(place_data)));
    }
    else
    {
        placed_once_m = true;

        place_data_m = place_data;

        RECT                    window_rect;
        std::pair<long, long>   extra(get_window_client_offsets(window_m));

        min_size_m.x_m = width(place_data);
        min_size_m.y_m = height(place_data);

        ::GetWindowRect(window_m, &window_rect);

        ::MoveWindow(   window_m, left(place_data) + window_rect.left,
                        top(place_data) + window_rect.top,
                        width(place_data) + extra.first,
                        height(place_data) + extra.second, TRUE);
    }
}

/****************************************************************************************************/

void window_t::set_size(const point_2d_t& size)
{
    assert(window_m);

    if (debounce_m) return;

    debounce_m = true;

    width(place_data_m) = size.x_m;
    height(place_data_m) = size.y_m;

    RECT                    window_rect;
    std::pair<long, long>   extra(get_window_client_offsets(window_m));

    ::GetWindowRect(window_m, &window_rect);

    ::SetWindowPos(window_m, 0, 0, 0,
                   width(place_data_m) + extra.first,
                   height(place_data_m) + extra.second,
                   SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER );

    debounce_m = false;
}

/****************************************************************************************************/

void window_t::reposition(window_reposition_t position)
{
    assert(window_m);

    RECT window_rect;

    implementation::get_control_bounds(window_m, window_rect);

    int width(window_rect.right - window_rect.left);
    int height(window_rect.bottom - window_rect.top);

    int sys_met_x(::GetSystemMetrics(SM_CXFULLSCREEN));
    int sys_met_y(::GetSystemMetrics(SM_CYFULLSCREEN));
    
    int left(std::max<int>(10, (sys_met_x - width)/2));
    int top;

    if (position == window_reposition_center_s)
        top = std::max<int>(10, (sys_met_y - height)/2);
    else //if (position == window_reposition_alert_s)
        top = std::max<int>(10, static_cast<int>((sys_met_y * .6 - height)/2));

    ::MoveWindow(window_m, left, top, width, height, TRUE);
}

/****************************************************************************************************/

void window_t::set_visible(bool make_visible)
{
    assert(window_m);

    if (IsWindowVisible(window_m) == false)
        reposition(window_reposition_center_s);

    set_control_visible(window_m, make_visible);

    ::EnableWindow(window_m, make_visible);
}

/****************************************************************************************************/

void window_t::monitor_resize(const window_resize_proc_t& proc)
{
    resize_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<window_t>(display_t&             display,
                                              platform_display_type& parent,
                                              window_t&       element)
{
    assert(!element.window_m);

    HWND parent_hwnd(parent);

    DWORD platform_style(WS_OVERLAPPED | WS_CAPTION | WS_BORDER/* | WS_SYSMENU*/);
    DWORD dialog_extended_style = WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME | WS_EX_COMPOSITED;

    if (element.attributes_m & (window_attributes_resizeable_s | window_attributes_live_resizeable_s))
        platform_style |= WS_SIZEBOX;

    element.window_m = ::CreateWindowExW(dialog_extended_style,
                                         L"eve_dialog",
                                         hackery::convert_utf(element.name_m).c_str(),
                                         platform_style,    
                                         10, 10, 20, 20,
                                         parent_hwnd,
                                         NULL,
                                         ::GetModuleHandle(NULL),
                                         NULL);

    if (element.window_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    set_user_reference(element.window_m, &element);

    return display.insert(parent, element.window_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
