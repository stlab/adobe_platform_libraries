/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_WINDOW_HPP
#define ADOBE_WIDGET_WINDOW_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <windows.h>

#include <adobe/any_regular.hpp>
#include <adobe/eve.hpp>
#include <adobe/extents.hpp>
#include <adobe/future/platform_primitives.hpp>
#include <adobe/future/widgets/headers/window_helper.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/function.hpp>

#include <string>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct window_t
{
    window_t(const std::string&  name,
             window_style_t      style,
             window_attributes_t attributes,
             window_modality_t   modality,
             theme_t             theme);

    ~window_t();

    void measure(extents_t& result);

    void place(const place_data_t& place_data);

    void set_visible(bool make_visible);

    void set_size(const point_2d_t& size);

    void reposition(window_reposition_t position);
 
    void monitor_resize(const window_resize_proc_t& proc);

    any_regular_t underlying_handler() { return any_regular_t(window_m); }

    bool handle_key(key_type /*key*/, bool /*pressed*/, modifiers_t /*modifiers*/)
        { return false; }

    HWND                 window_m;
    std::string          name_m;
    window_style_t       style_m;
    window_attributes_t  attributes_m;
    window_modality_t    modality_m;
    theme_t              theme_m;
    place_data_t         place_data_m;
    window_resize_proc_t resize_proc_m;
    bool                 debounce_m;
    point_2d_t           min_size_m;
    bool                 placed_once_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
