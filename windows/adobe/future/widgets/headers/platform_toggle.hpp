/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_TOGGLE_HPP
#define ADOBE_WIDGET_TOGGLE_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <windows.h>

#include <adobe/any_regular.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/function.hpp>
#include <boost/gil/gil_all.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct toggle_t
{
    typedef any_regular_t model_type;

    typedef boost::function<void (const model_type&)> setter_type;

    typedef boost::gil::rgba8_image_t image_type;

    toggle_t(const std::string&  alt_text,
             const any_regular_t value_on,
             const image_type&   image_on,
             const image_type&   image_off,
             const image_type&   image_disabled,
             theme_t             theme);

    void measure(extents_t& result);

    void place(const place_data_t& place_data);

    void monitor(const setter_type& proc);

    void enable(bool make_enabled);

    void display(const any_regular_t& to_value);

#ifndef ADOBE_NO_DOCUMENTATION
    HWND                       control_m;
    theme_t                    theme_m;
    std::string                alt_text_m;
    image_type                 image_on_m;
    image_type                 image_off_m;
    image_type                 image_disabled_m;
    setter_type                setter_proc_m;
    any_regular_t              value_on_m;
    any_regular_t              last_m;
    HBITMAP                    bitmap_on_m;
    HBITMAP                    bitmap_off_m;
    HBITMAP                    bitmap_disabled_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
