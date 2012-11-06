/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <windows.h>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

#include <adobe/algorithm.hpp>
#include <adobe/future/widgets/headers/platform_link.hpp>
#include <adobe/algorithm/copy.hpp>
#include <adobe/algorithm/copy.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/windows_graphic_utils.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>



/****************************************************************************************************/

namespace {

/****************************************************************************************************/

HBITMAP link_bitmap()
{
    static HBITMAP bitmap_s(0);

    if (bitmap_s == 0)
    {
        boost::gil::rgba8_image_t image;

        adobe::image_slurp("link_icon.tga", image);

        bitmap_s = adobe::to_bitmap(image);
    }

    return bitmap_s;
}

/****************************************************************************************************/

void draw_link(const adobe::link_t& link, HDC context)
{
    typedef std::vector<POINT> point_set_t;

    std::size_t num_prongs(link.prongs_m.size());

    if (!::IsWindowEnabled(link.control_m) || num_prongs <= 1)
        return;

    const long link_line_width(2);
    RECT       bounds = { 0 };

    ::GetClientRect(link.control_m, &bounds);

    // REVISIT (sparent) : If this were aligned top instead of fill then the hieght _should_ be correct.
    // REVISIT (sparent) : We should require up front (and default) to a count of 2.

    HPEN pen(::CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    HPEN old_pen(reinterpret_cast<HPEN>(::SelectObject(context, pen)));

    // REVISIT (sparent) FIXED VALUE
    bounds.right -= 6; // inset the rect a bit to create padding for the link icon

    for (std::size_t i(0); i < num_prongs; ++i)
    {
        long top(bounds.top + link.prongs_m[i] - link_line_width);

        RECT    fill_rect =
        {
            bounds.left,
            top,
            bounds.right,
            top + link_line_width
        };

        ::FillRect(context, &fill_rect, (HBRUSH) ::GetStockObject(BLACK_BRUSH));
    }

    // link spine
    RECT    fill_rect =
    {
        bounds.right,
        bounds.top + link.prongs_m[0] - link_line_width,
        bounds.right + link_line_width,
        link.prongs_m[num_prongs - 1] - link.prongs_m[0] + 1 * link_line_width
    };

    ::FillRect(context, &fill_rect, (HBRUSH) ::GetStockObject(BLACK_BRUSH));

    long center((fill_rect.bottom - fill_rect.top) / 2);

    ::MoveWindow(link.link_icon_m,
                 link.tl_m.x_m + 6,
                 link.tl_m.y_m + center - 8,
                 9, 16, TRUE);

    // clean up pen work
    SelectObject(context, old_pen);
    DeleteObject(pen);
}

/****************************************************************************************************/

LRESULT CALLBACK link_subclass_proc(HWND     window,
                                    UINT     message,
                                    WPARAM   wParam,
                                    LPARAM   lParam,
                                    UINT_PTR ptr,
                                    DWORD_PTR /* ref */)
{
    adobe::link_t& link(*reinterpret_cast<adobe::link_t*>(ptr));
    bool           handled(false);

    if ( message == WM_NCHITTEST )
    {
        handled = true;

        return HTCLIENT;
    }
    else if ( message == WM_LBUTTONDOWN )
    {
        link.hit_proc_m(link.off_value_m);

        RECT bounds;

        ::GetClientRect(window, &bounds);

        ::InvalidateRect(window, &bounds, TRUE);

        ::UpdateWindow(window);

        return 0;
    }
    else if (message == WM_PAINT)
    {
        PAINTSTRUCT paint_info = { 0 };

        ::BeginPaint(link.control_m, &paint_info); 
        draw_link(link, paint_info.hdc);
        ::EndPaint(link.control_m, &paint_info);
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

link_t::link_t(const std::string&          alt_text,
               const any_regular_t& on_value,
               const any_regular_t& off_value,
               long                        count,
               theme_t                     theme) :
    control_m(0),
    link_icon_m(0),
    alt_text_m(alt_text),
    on_value_m(on_value),
    off_value_m(off_value),
    count_m(count),
    theme_m(theme)
{ }

/****************************************************************************************************/

void link_t::measure(extents_t& result)
{
    result.width() = 15;
    result.height() = 5;

    for (long i(0); i < count_m; ++i)
        result.vertical().guide_set_m.push_back(2);
}

/****************************************************************************************************/

void link_t::place(const place_data_t& place_data)
{
    assert(control_m);

    tl_m.x_m = left(place_data);
    tl_m.y_m = top(place_data);

    implementation::set_control_bounds(control_m, place_data);

    prongs_m.erase(prongs_m.begin(), prongs_m.end());

    copy(place_data.vertical().guide_set_m, std::back_inserter(prongs_m));
}

/****************************************************************************************************/

void link_t::enable(bool make_enabled)
{
    assert(control_m);
    
    set_control_enabled(control_m, make_enabled);

    bool do_visible(value_m == on_value_m && make_enabled);

    set_control_visible(link_icon_m, do_visible);
    set_control_visible(control_m, do_visible);
}

/****************************************************************************************************/

void link_t::display(const any_regular_t& new_value)
{
    assert(control_m);
    
    value_m = new_value;

    bool do_visible(value_m == on_value_m && ::IsWindowEnabled(control_m));

    set_control_visible(link_icon_m, do_visible);
    set_control_visible(control_m, do_visible);
}

/****************************************************************************************************/

void link_t::monitor(const setter_type& proc)
{
    assert(control_m);

    hit_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<link_t>(display_t&             display,
                                            platform_display_type& parent,
                                            link_t&         element)
{
    HWND parent_hwnd(parent);

    element.control_m = ::CreateWindowExW(WS_EX_COMPOSITED, L"STATIC",
                                          NULL,
                                          WS_CHILD | WS_VISIBLE,
                                          0, 0, 50, 50,
                                          parent_hwnd,
                                          0,
                                          ::GetModuleHandle(NULL),
                                          NULL);

    if (element.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    if (!element.alt_text_m.empty())
        implementation::set_control_alt_text(element.control_m, element.alt_text_m);

    ::SetWindowSubclass(element.control_m, link_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);

    element.link_icon_m = ::CreateWindowExW(WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"STATIC",
                                            NULL,
                                            WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
                                            0, 0, 9, 16,
                                            parent_hwnd,
                                            0,
                                            ::GetModuleHandle(NULL),
                                            NULL);

    if (element.link_icon_m == NULL)
        ADOBE_THROW_LAST_ERROR;
    ::SendMessage(element.link_icon_m, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) link_bitmap());

    display.insert(parent, element.link_icon_m);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
