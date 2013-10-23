/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_progress_bar.hpp>

#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

void initialize(adobe::progress_bar_t& control, ::HIViewRef /*parent*/)
{
    assert(!control.control_m);

	static const ::Rect bounds_s = {0,0,100,100};
	
    if (control.bar_style_m == adobe::pb_style_relevance_bar_s)
        ADOBE_REQUIRE_STATUS(::CreateRelevanceBarControl(   0, &bounds_s,
                                                            0, 0, 100,
                                                            &control.control_m));
    else
        ADOBE_REQUIRE_STATUS(::CreateProgressBarControl(    0, &bounds_s,
                                                            0, 0, 100,
                                                            control.bar_style_m == adobe::pb_style_indeterminate_bar_s,
                                                            &control.control_m));

    adobe::implementation::set_theme(control.control_m, control.theme_m);

    ::SetControl32BitMinimum(control.control_m, static_cast< ::SInt32 >(0));
    ::SetControl32BitMaximum(control.control_m, static_cast< ::SInt32 >(control.format_m.size()));

    // REVISIT (fbrereto) : Setup fudge mods for vertical, horizontal bars & for various bar types
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

progress_bar_t::progress_bar_t(pb_style_t bar_style, 
                               bool is_vertical,
                               const value_range_format_t& format,
                               unsigned long /*min*/,
                               unsigned long /*max*/,
                               theme_t theme) :
    control_m(0),
    bar_style_m(bar_style),
    is_vertical_m(is_vertical),
    format_m(format),
    theme_m(theme)
{ }

/*************************************************************************************************/

void progress_bar_t::measure(extents_t& result)
{
    assert(control_m);

    result.height() = is_vertical_m ? 100 : 10;
    result.width() = is_vertical_m ? 10 : 100;
}

/*************************************************************************************************/

void progress_bar_t::place(const place_data_t& place_data)
{
    return implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void progress_bar_t::display(const ::SInt32& value)
{
    assert(control_m);

    ::SetControl32BitValue(control_m, static_cast< ::SInt32 >(value));
}

/****************************************************************************************************/

template <>
platform_display_type insert<progress_bar_t>(display_t&             display,
                                                    platform_display_type& parent,
                                                    progress_bar_t& element)
{
    ::HIViewRef parent_ref(parent);

    initialize(element, parent_ref);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
