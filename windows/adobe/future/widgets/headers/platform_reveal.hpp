/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_REVEAL_HPP
#define ADOBE_WIDGET_REVEAL_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>

#include <adobe/any_regular.hpp>

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct reveal_t : boost::noncopyable
{
    typedef any_regular_t model_type;

    typedef boost::function<void (const model_type&)> setter_type;

    reveal_t(const std::string&				name,
             const any_regular_t&	show_value,
             theme_t						theme,
             const std::string&				alt_text);

    void initialize(HWND parent);

    void measure(extents_t& result);

    void place(const place_data_t& place_data);

    void display(const any_regular_t& value);

    void monitor(const setter_type& proc);

    HWND                    control_m;
    theme_t          theme_m;
    label_t			name_m;
    bool                    using_label_m;
    setter_type				hit_proc_m;
    any_regular_t	show_value_m;
    any_regular_t	current_value_m;
    std::string				alt_text_m;
};

/****************************************************************************************************/

namespace view_implementation {

/****************************************************************************************************/

void set_value_from_model(reveal_t& value, const any_regular_t& new_value);

/****************************************************************************************************/

} // namespace view_implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
