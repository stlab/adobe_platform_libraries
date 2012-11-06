/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1

#include <windows.h>
#include <Commctrl.h>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/platform_slider.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

LRESULT CALLBACK slider_subclass_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR ptr, DWORD_PTR /* ref */)
{
    adobe::slider_t& control(*reinterpret_cast<adobe::slider_t*>(ptr));

    assert(control.control_m);

    if (control.value_proc_m.empty() == false &&
        (message == WM_HSCROLL || message == WM_VSCROLL))
    {
        WORD submsg(LOWORD(wParam));

        if (submsg == TB_LINEUP        || submsg == TB_LINEDOWN   ||
            submsg == TB_PAGEUP        || submsg == TB_PAGEDOWN   ||
            submsg == TB_THUMBPOSITION || submsg == TB_THUMBTRACK ||
            submsg == TB_TOP           || submsg == TB_BOTTOM     ||
            submsg == TB_ENDTRACK)
        {
            long   new_position(static_cast<long>(::SendMessage(window, TBM_GETPOS, 0, 0)));
            double new_value(control.format_m.at(new_position).cast<double>());

            if (new_value != control.value_m)
            {
                control.value_m = new_value;

                control.value_proc_m(static_cast<adobe::slider_t::model_type>(control.value_m));
            }
        }
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

void initialize(adobe::slider_t& control, HWND parent)
{
    assert(!control.control_m);

    DWORD win32_style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    win32_style |= control.is_vertical_m ? TBS_VERT : TBS_HORZ;

    if (control.num_ticks_m)
    {
        switch (control.style_m)
        {
            case adobe::slider_points_up_s:    win32_style |= TBS_TOP;    break;
            case adobe::slider_points_left_s:  win32_style |= TBS_LEFT;   break;
            case adobe::slider_points_down_s:  win32_style |= TBS_BOTTOM; break;
            case adobe::slider_points_right_s: win32_style |= TBS_RIGHT;  break;
            default: break; // silences a GCC warning
        }
    }
    else
    {
        win32_style |= TBS_NOTICKS;
    }

    control.control_m = ::CreateWindowEx(WS_EX_COMPOSITED,
                                         TRACKBAR_CLASS,
                                         NULL,
                                         win32_style,
                                         0, 0, 20, 20,
                                         parent,
                                         0,
                                         ::GetModuleHandle(NULL),
                                         NULL);

    if (control.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    adobe::set_font(control.control_m, control.is_vertical_m ? TKP_THUMBTOP : TKP_THUMBLEFT);

    ::SetWindowSubclass(control.control_m, slider_subclass_proc, reinterpret_cast<UINT_PTR>(&control), 0);
	::SendMessage (control.control_m, TBM_SETRANGE, FALSE, MAKELONG(0, control.format_m.size()));
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
    int theme_type(is_vertical_m ? TKP_THUMBTOP : TKP_THUMBLEFT);

    //
    // Get the size of the thumb, and then multiply it by the number of tick
    // marks.
    //
    SIZE thumb_size;

    if (!metrics::set_window(control_m))
        ADOBE_THROW_LAST_ERROR;

    if (!metrics::get_size(theme_type, TS_TRUE, thumb_size))
        ADOBE_THROW_LAST_ERROR;

    result.width()  = is_vertical_m ? thumb_size.cx : thumb_size.cx * 10;
    result.height() = is_vertical_m ? thumb_size.cy * 10 : thumb_size.cy;

    long extra(7 + (num_ticks_m ? 6 : 0));

    // Add extra space to accomodate the tick marks. Even in the case when they are not
    // drawn, they are still accounted for in the widget.
    (is_vertical_m ? result.width() : result.height()) += extra;

    //
    // Add on any border.
    //
    int border(0);
    metrics::get_integer(theme_type, TMT_BORDERSIZE, border);

    result.width() += 2 * border;
    result.height() += 2 * border;
}

/****************************************************************************************************/

void slider_t::place(const place_data_t& place_data)
{
    implementation::set_control_bounds(control_m, place_data);
}

/****************************************************************************************************/

void slider_t::enable(bool make_enabled)
{
    assert(control_m);

    ::EnableWindow(control_m, make_enabled);
}

/****************************************************************************************************/

void slider_t::display(const model_type& value)
{
    value_m = value;
    
    long new_position(long(format_m.find(any_regular_t(value))));

    if (new_position != last_m)
    {
        last_m = new_position;

        ::SendMessage(control_m, TBM_SETPOS, true, static_cast<LPARAM>(new_position)); 
    }
}

/****************************************************************************************************/

template <>
platform_display_type insert<slider_t>(display_t& display, platform_display_type& parent, slider_t& element)
{
    HWND parent_hwnd(parent);

    initialize(element, parent_hwnd);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
