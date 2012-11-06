/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_CHECKBOX_HPP
#define ADOBE_WIDGET_CHECKBOX_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>

#include <adobe/any_regular.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/function.hpp>

#include <string>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*
    \omit from the doxygen documentation (for now)

    \ingroup asl_widgets_win32

    \brief UI core checkbox widget

    Further documentation can be found in the \ref asl_widgets_carbon.
*/

struct checkbox_t
{
    typedef any_regular_t                      model_type;
    typedef boost::function<void (const model_type&)> setter_type;

    checkbox_t(const std::string&          name,
               const any_regular_t& true_value,
               const any_regular_t& false_value,
               theme_t              theme,
               const std::string&          alt_text);

    void measure(extents_t& result);

    void place(const place_data_t& place_data);

    void enable(bool make_enabled);

    void display(const any_regular_t& value);

    void monitor(const setter_type& proc);

    friend bool operator == (const checkbox_t& x, const checkbox_t& y);

    HWND                 control_m;
    theme_t       theme_m;
    setter_type          hit_proc_m;
    any_regular_t true_value_m;
    any_regular_t false_value_m;
    any_regular_t current_value_m;
    std::string          name_m;
    std::string          alt_text_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
