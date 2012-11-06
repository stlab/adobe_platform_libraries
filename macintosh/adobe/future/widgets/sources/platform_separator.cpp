/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_separator.hpp>
#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

separator_t::separator_t(bool is_vertical, theme_t theme)
    : control_m(0), is_vertical_m(is_vertical), theme_m(theme)
{

}

/****************************************************************************************************/

void separator_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void separator_t::set_visible(bool make_visible)
{ 
    implementation::set_visible(control_m, make_visible); 
}

/****************************************************************************************************/

void separator_t::measure(extents_t& result)
{ 
    result.horizontal().length_m = is_vertical_m ? 5 : 6;
    result.vertical().length_m  = is_vertical_m ? 6 : 5;
}
    
/****************************************************************************************************/

template <>
platform_display_type insert<separator_t>(display_t&             display,
                                                 platform_display_type&  parent,
                                                 separator_t&     element)
{

    assert(!element.control_m);
    
    const ::Rect bounds = { 0, 0, 10, 10 };
    
    ADOBE_REQUIRE_STATUS(::CreateSeparatorControl(0, &bounds, &element.control_m));

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/
}
