/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP
#define ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/macintosh_mouse_wheel_handler.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct edit_number_t;

/****************************************************************************************************/

struct edit_number_platform_data_t
{
    explicit edit_number_platform_data_t(edit_number_t* edit_number) :
        control_m(edit_number)
    { }

    edit_number_platform_data_t(const edit_number_platform_data_t& rhs) :
        control_m(rhs.control_m)
        // wheel_handler_m not shareable
    { }

    edit_number_platform_data_t& operator= (const edit_number_platform_data_t& rhs)
    {
        control_m = rhs.control_m;
        // wheel_handler_m not shareable

        return *this;
    }

    void initialize();

private:
    void wheel(long delta, bool extra);
    void label_subevent(label_subevent_t type);
    void scrubby(double new_value);

    edit_number_t*        control_m;
    mouse_wheel_handler_t wheel_handler_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP
#endif

/****************************************************************************************************/
