/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_SEPARATOR_HPP
#define ADOBE_SEPARATOR_HPP

#include <adobe/config.hpp>

#include <boost/utility.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
    
struct separator_t : boost::noncopyable, extents_slices_t
{    
    separator_t(bool is_vertical, theme_t theme);
    
    void            measure(extents_t& result);
    
    void            place(const place_data_t& place_data);
    
    void            set_visible(bool make_visible);
    
    HWND            control_m;
    bool            is_vertical_m;
    theme_t  theme_m;
};

/****************************************************************************************************/

}

/****************************************************************************************************/

#endif
