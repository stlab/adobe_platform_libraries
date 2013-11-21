/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP
#define ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct edit_number_t;

/****************************************************************************************************/

struct edit_number_platform_data_t
{
    explicit edit_number_platform_data_t(edit_number_t* edit_number) :
        control_m(edit_number),
        prev_capture_m(0),
        tracking_m(false)
    {
        last_point_m.x = 0;
        last_point_m.y = 0;
    }

    ~edit_number_platform_data_t();

    void initialize();

    LRESULT label_message(UINT message, WPARAM wParam, LPARAM lParam);

    edit_number_t* control_m;
    HWND           prev_capture_m;
    bool           tracking_m;
    POINTS         last_point_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_WIDGET_EDIT_NUMBER_EDGE_HPP
#endif

/****************************************************************************************************/
