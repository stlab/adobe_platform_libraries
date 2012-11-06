/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <windows.h>
#include <uxtheme.h>
#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

#include <adobe/future/widgets/headers/platform_image.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/future/windows_graphic_utils.hpp>
#include <adobe/memory.hpp>
#include <adobe/unicode.hpp>

#include <string>
#include <cassert>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const long fixed_width = 250;
const long fixed_height = fixed_width;

/****************************************************************************************************/

void reset_image(HWND window, const adobe::image_t::view_model_type& view)
{
    HBITMAP bitmap_handle;

    bitmap_handle = adobe::to_bitmap(view);

    HBITMAP old_bm_handle = reinterpret_cast<HBITMAP>(
        ::SendMessage(window, STM_SETIMAGE, IMAGE_BITMAP, hackery::cast<LPARAM>(bitmap_handle)));

    HRESULT result = S_OK;
    if(old_bm_handle) {
        result = ::DeleteObject(reinterpret_cast<HBITMAP>(old_bm_handle));

    }
    if(result != S_OK) ADOBE_THROW_LAST_ERROR;

/*
    HBITMAP new_bm_handle = 
        reinterpret_cast<HBITMAP>(::SendMessage(window, STM_GETIMAGE, IMAGE_BITMAP, 0));
    
   This idea doesn't seem to work. Was trying to detect alpha pixel case so as to 
   be able to delete orgibal bitmap if it was in fact copied. But result comes back
   as S_FALSE, so need a different technique. 
    if(new_bm_handle != bitmap_handle)
        result = ::DeleteObject(bitmap_handle);
    if(result != S_OK) ADOBE_THROW_LAST_ERROR;
*/ 
    
}

/****************************************************************************************************/

extern const char implementation_proc_name[] = "adobe_widgets_image_proc_handler";

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

image_t::image_t(const view_model_type& image) :
    window_m(0),
    image_m(image),
    tracking_m(false),
    enabled_m(false)
{
    handler_m.reset(new message_handler_t(boost::bind(&image_t::handle_event,
                                                      boost::ref(*this), _1, _2, _3, _4, _5)));

    metadata_m.insert(dictionary_t::value_type(static_name_t("delta_x"), any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type(static_name_t("delta_y"), any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type(static_name_t("dragging"), any_regular_t(false)));
    metadata_m.insert(dictionary_t::value_type(static_name_t("x"), any_regular_t(0)));
    metadata_m.insert(dictionary_t::value_type(static_name_t("y"), any_regular_t(0)));
}

/****************************************************************************************************/

LRESULT image_t::handle_event(HWND window, UINT message, WPARAM wparam, LPARAM lparam, WNDPROC next_proc)
{
    if (message == WM_NCHITTEST)
    {
        return HTCLIENT;
    }
    else if (message == WM_MOUSEMOVE && tracking_m)
    {
        POINTS cur_point(MAKEPOINTS(lparam));

        if (last_point_m.y != cur_point.y ||
            last_point_m.x != cur_point.x)
        {
            double x(0);
            double y(0);
            double delta_x(last_point_m.x - cur_point.x);
            double delta_y(last_point_m.y - cur_point.y);

            get_value(metadata_m, static_name_t("x"), x);
            get_value(metadata_m, static_name_t("y"), y);

            metadata_m.insert(dictionary_t::value_type(static_name_t("delta_x"), any_regular_t(delta_x)));
            metadata_m.insert(dictionary_t::value_type(static_name_t("delta_y"), any_regular_t(delta_y)));
            metadata_m.insert(dictionary_t::value_type(static_name_t("dragging"), any_regular_t(true)));
            metadata_m.insert(dictionary_t::value_type(static_name_t("x"), any_regular_t(x + delta_x)));
            metadata_m.insert(dictionary_t::value_type(static_name_t("y"), any_regular_t(y + delta_y)));

            callback_m(metadata_m);
        }

        last_point_m = cur_point;
    }
    else if (message == WM_LBUTTONDOWN)
    {
        tracking_m = true;
        prev_capture_m = ::SetCapture(window_m);
        last_point_m = MAKEPOINTS(lparam);

        return 0;
    }
    else if (message == WM_LBUTTONUP)
    {
        tracking_m = false;
        ::SetCapture(prev_capture_m);

		metadata_m.insert(dictionary_t::value_type(static_name_t("delta_x"), any_regular_t(0)));
        metadata_m.insert(dictionary_t::value_type(static_name_t("delta_y"), any_regular_t(0)));
        metadata_m.insert(dictionary_t::value_type(static_name_t("dragging"), any_regular_t(false)));

		callback_m(metadata_m);

        return 0;
    }

    return ::CallWindowProc(next_proc, window, message, wparam, lparam);
}

/****************************************************************************************************/

void image_t::display(const view_model_type& value)
{
    image_m = value;

    if (image_m.width() <= fixed_width)
        origin_m.first = 0;
    else
        origin_m.first = static_cast<long>((image_m.width() - fixed_width) / 2);

    if (image_m.height() <= fixed_height)
        origin_m.second = 0;
    else
        origin_m.second = static_cast<long>((image_m.height() - fixed_height) / 2);

    reset_image(window_m, image_m);
}

/****************************************************************************************************/

void image_t::monitor(const setter_proc_type& proc)
{
    callback_m = proc;
}

/****************************************************************************************************/

void image_t::enable(bool make_enabled)
{
    enabled_m = make_enabled;
}

/****************************************************************************************************/
void initialize(image_t& value, HWND parent)
{
    value.window_m = ::CreateWindowExA(WS_EX_COMPOSITED, "STATIC",
                                       NULL,
                                       WS_CHILD | WS_VISIBLE | SS_BITMAP,
                                       0, 0,
                                       hackery::cast<int>(value.image_m.width()),
                                       hackery::cast<int>(value.image_m.height()),
                                       parent,
                                       NULL,
                                       ::GetModuleHandle(NULL),
                                       NULL);

    if (value.window_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    value.handler_m->install<implementation_proc_name>(value.window_m);

    // now set up the bitmap

    reset_image(value.window_m, value.image_m);
}

/****************************************************************************************************/

void place(image_t& value, const place_data_t& place_data)
{
    implementation::set_control_bounds(value.window_m, place_data);

    if (value.callback_m)
    {
        dictionary_t old_metadata(value.metadata_m);

        double width(std::min<double>(fixed_width, value.image_m.width()));
        double height(std::min<double>(fixed_height, value.image_m.height()));

        value.metadata_m.insert(dictionary_t::value_type(static_name_t("width"), any_regular_t(width)));
        value.metadata_m.insert(dictionary_t::value_type(static_name_t("height"), any_regular_t(height)));

        if (old_metadata != value.metadata_m)
            value.callback_m(value.metadata_m);
    }
}

/****************************************************************************************************/

void measure(image_t& value, extents_t& result)
{
    if (value.callback_m)
        result.horizontal().length_m = fixed_width;
    else
        result.horizontal().length_m = (long)value.image_m.width();
}

/****************************************************************************************************/

void measure_vertical(image_t& value, extents_t& result, const place_data_t& placed_horizontal)
{
    if (value.callback_m)
        result.vertical().length_m = fixed_height;
    else
    {
        // NOTE (fbrereto) : We calculate and use the aspect ratio here to
        //                   maintain a proper initial height and width in
        //                   the case when the image grew based on how it
        //                   is being laid out.

        float aspect_ratio(value.image_m.height() / static_cast<float>(value.image_m.width()));

        result.vertical().length_m = static_cast<long>(placed_horizontal.horizontal().length_m * aspect_ratio);
    }
}

/****************************************************************************************************/

void enable(image_t& value, bool make_enabled)
{
    ::EnableWindow(value.window_m, make_enabled);
}

/****************************************************************************************************/

void set(image_t& value, boost::gil::rgba8_image_t& image)
{
    value.image_m = image;

    reset_image(value.window_m, value.image_m);
}

/*************************************************************************************************/

template <>
platform_display_type insert<image_t>(display_t&             display,
                                             platform_display_type& parent,
                                             image_t&        element)
{
    HWND parent_hwnd(parent);
    initialize(element, parent_hwnd);

    return display.insert(parent, get_display(element));
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
