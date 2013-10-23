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

#include <adobe/future/number_formatter.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_display_number.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/unicode.hpp>

#include <string>
#include <cassert>
#include <sstream>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

std::wstring set_field_text(std::string                       label,
                            double                            value,
                            const std::vector<adobe::unit_t>& unit_set,
                            std::wstring::size_type*          label_chars=0)
{
    std::stringstream result;

    // this is called by a subview when its value changes.
    // we are only concerned with changing our actual value when
    // the view that changed is also the current view.

    std::string               suffix;
    adobe::number_formatter_t number_formatter;

    if (!unit_set.empty())
    {
        std::vector<adobe::unit_t>::const_iterator i(unit_set.begin());
        std::vector<adobe::unit_t>::const_iterator end(unit_set.end());

        while (i != end && i->scale_m_m <= value)
            ++i;

        if (i != unit_set.begin())
            --i;

        value = adobe::to_base_value(value, *i);

        suffix = i->name_m;

        number_formatter.set_format(i->format_m);
    }

    if (!label.empty())
    {
        result << label << " ";

        if(label_chars)
            *label_chars = label.length();
    }

    result << number_formatter.format(value);

    if (!suffix.empty())
        result << " " << suffix;

    return hackery::convert_utf(result.str().c_str());
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

extern void throw_last_error_exception(const char* file, long line);

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

void display_number_t::initialize(HWND parent)
{
    RECT bounds = { 0, 0, 100, 100 };

    bounds_m = bounds;

    long width(bounds.right - bounds.left);
    long height(bounds.bottom - bounds.top);

    window_m = ::CreateWindowExW(WS_EX_COMPOSITED, L"STATIC",
                                 NULL,
                                 WS_CHILD | WS_VISIBLE,
                                 bounds.left, bounds.top, width, height,
                                 parent,
                                 NULL,
                                 ::GetModuleHandle(NULL),
                                 NULL);

    if (window_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    if (!alt_text_m.empty())
        implementation::set_control_alt_text(window_m, alt_text_m);

    set_font(window_m, EP_EDITTEXT);
}

/****************************************************************************************************/

void display_number_t::place(const place_data_t& place_data)
{
    implementation::set_control_bounds(window_m, place_data);
}

/****************************************************************************************************/

void display_number_t::display(const model_type& value)
{
    assert(window_m);

    ::SetWindowTextW(window_m, set_field_text(name_m, value, unit_set_m).c_str());
}

/****************************************************************************************************/

void display_number_t::measure(extents_t& result)
{
    assert(window_m);

    extents_t space_extents(metrics::measure_text(std::string(" "), window_m, EP_EDITTEXT));
    extents_t unit_max_extents;
    extents_t label_extents(metrics::measure_text(name_m, window_m, EP_EDITTEXT));
    extents_t characters_extents =
        metrics::measure_text(std::string(characters_m, '0'), window_m, EP_EDITTEXT);

    for (display_number_t::unit_set_t::iterator iter(unit_set_m.begin()),
         end(unit_set_m.end()); iter != end; ++iter)
        {
            extents_t tmp(metrics::measure_text(iter->name_m, window_m, EP_EDITTEXT));

            if (tmp.width() > unit_max_extents.width())
                unit_max_extents = tmp;
        }

    // set up default settings (baseline, etc.)
    result = space_extents;

    // set the width to the label width (if any)
    result.width() = label_extents.width();

    // add a guide for the label
    result.horizontal().guide_set_m.push_back(label_extents.width());

    // if there's a label, add space for a space
    if (label_extents.width() != 0)
        result.width() += space_extents.width();

    // append the character extents (if any)
    result.width() += characters_extents.width();

    // if there are character extents, add space for a space
    if (characters_extents.width() != 0)
        result.width() += space_extents.width();

    // append the max unit extents (if any)
    result.width() += unit_max_extents.width();

    assert(result.horizontal().length_m);
}

/****************************************************************************************************/

void display_number_t::measure_vertical(extents_t& calculated_horizontal, const place_data_t& placed_horizontal)
{
    assert(window_m);

    RECT save_bounds;

    implementation::get_control_bounds(window_m, save_bounds);

    place_data_t static_bounds;

    top(static_bounds) = top(placed_horizontal);
    left(static_bounds) = left(placed_horizontal);
    width(static_bounds) = width(placed_horizontal);
    height(static_bounds) = 10000; // bottomless

    implementation::set_control_bounds(window_m, static_bounds);

	HDC hdc(::GetWindowDC(window_m));
    std::string title(implementation::get_window_title(window_m));

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
 
    metrics::set_window(window_m);

    if (have_tm)
        // distance from top to baseline
        vert.guide_set_m.push_back(widget_tm.tmHeight - widget_tm.tmDescent);

    place_data_t restore_bounds;

    top(restore_bounds) = save_bounds.top;
    left(restore_bounds) = save_bounds.left;
    width(restore_bounds) = save_bounds.right - save_bounds.left;
    height(restore_bounds) = save_bounds.bottom - save_bounds.top;

    implementation::set_control_bounds(window_m, restore_bounds);
}

/****************************************************************************************************/

// REVISIT: MM--we need to replace the display_t mechanism with concepts/any_*/container idiom for event and drawing system.

template <>
platform_display_type insert<display_number_t>(display_t& display,
                                             platform_display_type& parent,
                                             display_number_t& element)
{
    HWND parent_hwnd(parent);
    element.initialize(parent_hwnd);
    return display.insert(parent, get_display(element));
}


/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
