/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_reveal.hpp>

#include <adobe/empty.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_graphic_utils.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/placeable_concept.hpp>

#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

HBITMAP bitmap_showing()
{
    static HBITMAP bitmap_s(0);

    if (bitmap_s == 0)
    {
        boost::gil::rgba8_image_t image;

        adobe::image_slurp("windows_reveal_down.tga", image);

        bitmap_s = adobe::to_bitmap(image);
    }

    return bitmap_s;
}

/****************************************************************************************************/

HBITMAP bitmap_hidden()
{
    static HBITMAP bitmap_s(0);

    if (bitmap_s == 0)
    {
        boost::gil::rgba8_image_t image;

        adobe::image_slurp("windows_reveal_up.tga", image);

        bitmap_s = adobe::to_bitmap(image);
    }

    return bitmap_s;
}

/****************************************************************************************************/

LRESULT CALLBACK reveal_subclass_proc(HWND     window,
                                      UINT     message,
                                      WPARAM   wParam,
                                      LPARAM   lParam,
                                      UINT_PTR ptr,
                                      DWORD_PTR /* ref */)
{
    adobe::reveal_t& reveal(*reinterpret_cast<adobe::reveal_t*>(ptr));

    if (message == WM_COMMAND && HIWORD(wParam) == STN_CLICKED)
    {
        if (reveal.hit_proc_m.empty())
            return 0;

        // toggle it.
        adobe::any_regular_t new_value =
            reveal.current_value_m == reveal.show_value_m ?
                adobe::any_regular_t(adobe::empty_t()) :
                reveal.show_value_m;

        reveal.hit_proc_m(new_value);

        return 0;
    }

    // nevermind.
    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/


reveal_t::reveal_t(const std::string&			name,
                   const any_regular_t&	show_value,
                   theme_t						theme,
                   const std::string&			alt_text) :
    control_m(0),
    theme_m(theme),
    name_m(name, std::string(), 0, theme),
    using_label_m(!name.empty()),
    show_value_m(show_value),
    alt_text_m(alt_text)
{
}


/****************************************************************************************************/

void reveal_t::initialize(HWND parent)
{
    assert(!control_m);

    control_m = ::CreateWindowExW(  WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"STATIC",
                                    NULL,
                                    WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
                                    0, 0, 100, 20,
                                    parent,
                                    0,
                                    ::GetModuleHandle(NULL),
                                    NULL);

    if (control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    set_font(control_m, EP_EDITTEXT); // REVISIT (fbrereto) : a better type?

    ::SetWindowSubclass(control_m, &reveal_subclass_proc, reinterpret_cast<UINT_PTR>(this), 0);

    if (!alt_text_m.empty())
        implementation::set_control_alt_text(control_m, alt_text_m);
}

/****************************************************************************************************/

void reveal_t::measure(extents_t& result)
{
    using adobe::measure;

    // REVISIT (fbrereto) : hardwired defaults
    result.width() = 16;
    result.height() = 17;

    if (!using_label_m)
        return;

    extents_t label_extents;

    measure(name_m, label_extents);

    place_data_t label_place;

    width(label_place) = label_extents.width();
    height(label_place) = label_extents.height();

    measure_vertical(name_m, label_extents, label_place);

    result.width() += 4 /* gap */ + label_extents.width();
    result.height() = (std::max)(result.height(), label_extents.height());
}

/****************************************************************************************************/

void reveal_t::place(const place_data_t& place_data)
{
    using adobe::place;

    assert(control_m);

    if (!using_label_m)
    {
        implementation::set_control_bounds(control_m, place_data);
    }
    else
    {
        place_data_t tmp(place_data);

        // REVISIT (fbrereto) : hardwired defaults
        width(tmp) = 16;
        height(tmp) = 17;

        implementation::set_control_bounds(control_m, tmp);

        width(tmp) = width(place_data) - 16 - 4 /* gap */;
        left(tmp) = left(place_data) + 16 + 4 /* gap */;

        place(name_m, tmp);
    }
}

/****************************************************************************************************/

void reveal_t::display(const any_regular_t& new_value)
{
    assert(control_m);

    if (current_value_m == new_value)
        return;

    current_value_m = new_value;

    HBITMAP new_map = current_value_m == show_value_m ? bitmap_showing() : bitmap_hidden();

    ::SendMessage(control_m, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) new_map);
}

/****************************************************************************************************/

void reveal_t::monitor(const setter_type& proc)
{
    assert(control_m);

    hit_proc_m = proc;
}

/****************************************************************************************************/      

// REVISIT: MM--we need to replace the display_t mechanism with concepts/any_*/container idiom for event and drawing system.

template <>
platform_display_type insert<reveal_t>(display_t&             display,
                                              platform_display_type& parent,
                                              reveal_t&       element)
{
    HWND parent_hwnd(parent);

    element.initialize(parent_hwnd);

	platform_display_type result(display.insert(parent, element.control_m));

	if (element.using_label_m){
		initialize(element.name_m, parent_hwnd);
        display.insert(parent, get_display(element.name_m));
	}

    return result;
}


/****************************************************************************************************/

}


/****************************************************************************************************/
