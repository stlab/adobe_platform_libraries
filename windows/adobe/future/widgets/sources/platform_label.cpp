/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <windows.h>
#include <uxtheme.h>
#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file


#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/unicode.hpp>
#include <adobe/placeable_concept.hpp>
#include <string>
#include <cassert>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

LRESULT CALLBACK label_subclass_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR /* ptr */, DWORD_PTR /* ref */)
{
#ifndef NDEBUG
    if (message == WM_SHOWWINDOW)
    {
        bool shown = wParam == 0;
    }
#endif

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

label_t::label_t(const std::string& name,
                 const std::string& alt_text,
                 std::size_t        characters,
                 theme_t            theme) :
    window_m(0),
    characters_m(characters),
    theme_m(theme),
    alt_text_m(alt_text),
	name_m(name)
{
}

/****************************************************************************************************/

label_t::~label_t()
{
    ::DestroyWindow(window_m);
}

/****************************************************************************************************/

void place(label_t& value, const place_data_t& place_data)
{
    implementation::set_control_bounds(value.window_m, place_data);

    if (!value.alt_text_m.empty())
        implementation::set_control_alt_text(value.window_m, value.alt_text_m);
}

/****************************************************************************************************/

void measure(label_t& value, extents_t& result)
{
    assert(value.window_m);

    metrics::set_window(value.window_m);
//    metrics::set_theme_name(L"Edit");
    //
    // If we don't have the type of this widget, then we should return a
    // zero sized rectangle. This is usually correct, and a good assumption
    // anyway.
    //
    int uxtheme_type = EP_EDITTEXT;
    //
    // Discover the size of the widget.
    //
    SIZE widget_size;
    bool have_size = metrics::get_size(uxtheme_type, TS_DRAW, widget_size);
    //
    // Discover any margins of this widget.
    //
    MARGINS widget_margins;
    bool have_margins = metrics::get_margins(uxtheme_type, widget_margins);
    //
    // Get the text metrics (and calculate the baseline of this widget)
    //
    TEXTMETRIC widget_tm;
    bool have_tm = metrics::get_font_metrics(uxtheme_type, widget_tm);
    //
    // We need the text dimensions to figure out what the width of the widget should
    // be.
    //
    RECT text_extents;
    RECT in_extents = {0,0,10000,10000};
    
    if(value.characters_m){
        std::wstring wrap(value.characters_m * 2, L'0');
        bool have_extents = metrics::get_text_extents(uxtheme_type,
            wrap.c_str(), text_extents, &in_extents);
        assert(have_extents);
        in_extents = text_extents;
        in_extents.bottom = 10000;
    }
    
    std::wstring wname;
    to_utf16(value.name_m.begin(), value.name_m.end(), back_inserter(wname));
    
    bool have_extents = metrics::get_text_extents(uxtheme_type,
        wname.c_str(), text_extents, &in_extents);


    //
    // Get any border the widget may have.
    //
    int border;
    bool have_border = metrics::get_integer(uxtheme_type, TMT_BORDERSIZE, border);
    //
    // Now we can calculate the size we want to return.
    //
    result.horizontal().length_m = text_extents.right - text_extents.left;
    assert(result.horizontal().length_m);
}

/****************************************************************************************************/

void measure_vertical(label_t& value, extents_t& calculated_horizontal, 
                      const place_data_t& placed_horizontal)
{
    assert(value.window_m);

    RECT save_bounds;

    implementation::get_control_bounds(value.window_m, save_bounds);

    place_data_t static_bounds;

    top(static_bounds) = top(placed_horizontal);
    left(static_bounds) = left(placed_horizontal);
    width(static_bounds) = width(placed_horizontal);
    height(static_bounds) = 10000; // bottomless

    implementation::set_control_bounds(value.window_m, static_bounds);

	HDC hdc(::GetWindowDC(value.window_m));
    std::string title(implementation::get_window_title(value.window_m));

    std::wstring wtitle;
    to_utf16(title.begin(), title.end(), std::back_inserter(wtitle));
    RECT out_extent;

//    metrics::set_theme_name(L"Edit");
    //
    // If we don't have the type of this widget, then we should return a
    // zero sized rectangle. This is usually correct, and a good assumption
    // anyway.
    //
    int uxtheme_type = EP_EDITTEXT;
    //
    // Get the text metrics (and calculate the baseline of this widget)
    //
    TEXTMETRIC widget_tm;
    bool have_tm = metrics::get_font_metrics(uxtheme_type, widget_tm);

    assert(have_tm);

    const RECT in_extents =
    {
        left(static_bounds),
        top(static_bounds),
        right(static_bounds),
        bottom(static_bounds)
    };

    bool have_extents = metrics::get_text_extents(uxtheme_type,
        wtitle.c_str(), out_extent, &in_extents);
    
    assert(have_extents);

    extents_t::slice_t& vert = calculated_horizontal.vertical();
    vert.length_m = out_extent.bottom - out_extent.top;
    // set the baseline for the text
 
    metrics::set_window(value.window_m);

    if (have_tm)
        // distance from top to baseline
        vert.guide_set_m.push_back(widget_tm.tmHeight - widget_tm.tmDescent);

    place_data_t restore_bounds;

    top(restore_bounds) = save_bounds.top;
    left(restore_bounds) = save_bounds.left;
    width(restore_bounds) = save_bounds.right - save_bounds.left;
    height(restore_bounds) = save_bounds.bottom - save_bounds.top;

    implementation::set_control_bounds(value.window_m, restore_bounds);
}

/****************************************************************************************************/

void enable(label_t& value, bool make_enabled)
{
    ::EnableWindow(value.window_m, make_enabled);
}

/****************************************************************************************************/

void initialize(label_t& label, HWND parent)
{
    assert(!label.window_m);

    label.window_m = ::CreateWindowExW(WS_EX_COMPOSITED, L"STATIC",
                                       hackery::convert_utf(label.name_m).c_str(),
                                       WS_CHILD | WS_VISIBLE,
                                       0, 0, 100, 20,
                                       parent,
                                       NULL,
                                       ::GetModuleHandle(NULL),
                                       NULL);

    if (label.window_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    set_font(label.window_m, EP_EDITTEXT);

    if (!label.alt_text_m.empty())
        implementation::set_control_alt_text(label.window_m, label.alt_text_m);

    ::SetWindowSubclass(label.window_m, &label_subclass_proc, reinterpret_cast<UINT_PTR>(&label), 0);
}

/****************************************************************************************************/

extents_t measure_text(const std::string& text, theme_t theme, HWND temp_parent)
{
    label_t          label(text, std::string(), 0, theme);
    extents_t result;

    measure_label_text(label, result, temp_parent);

    return result;
}

/****************************************************************************************************/

void measure_label_text(const label_t& label, extents_t& result, HWND temp_parent)
{
    label_t&  temp(const_cast<label_t&>(label));

    if (temp.window_m == NULL)
        initialize(temp, temp_parent);

    measure(temp, result);

    place_data_t p;
    p.horizontal().length_m = result.width();
    measure_vertical(temp, result, p);
}

/****************************************************************************************************/

std::string get_control_string(const label_t& widget)
{
    return implementation::get_control_string(widget.window_m);
}

/****************************************************************************************************/

// REVISIT: MM--we need to replace the display_t mechanism with concepts/any_*/
//              container idiom for event and drawing system.

template <>
platform_display_type insert<label_t>(display_t& display,
                                             platform_display_type& parent,
                                             label_t& element)
{
    HWND parent_hwnd(parent);

    initialize(element, parent_hwnd);

    return display.insert(parent, get_display(element));
}


/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
