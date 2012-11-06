/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_PROGRESS_BAR_HPP
#define ADOBE_PROGRESS_BAR_HPP

#include <adobe/config.hpp>

#include <boost/utility.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/value_range_format.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!
\ingroup ui_core

\brief Specifies the style of progress bar to be presented to the user

\note
    Could probably be refactored to specify the intention for the
    progress bar instead of the end-look-and-feel of the progress bar.
*/

enum pb_style_t
{
    /// Typical progress bar intention; repeatedly updated to reflect
    /// the progress of an ongoing task
    pb_style_progress_bar_s,

    /// Updated once (typically) to reflect an amount a certain value
    /// relates to another value. For instance, the relevance of a
    /// search result to the search criterion
    pb_style_relevance_bar_s,

    /// "Barber"-style animation of a bar; repeatedly animated to
    /// reflect the fact that while we don't know how long the task
    /// will take to complete, we can tell you that the app isn't
    /// frozen, it's just thinking hard
    pb_style_indeterminate_bar_s
};

/****************************************************************************************************/

struct progress_bar_t : boost::noncopyable
{
    typedef     progress_bar_t   widget_type_t;
    typedef     any_regular_t    model_type;
    
                progress_bar_t(pb_style_t bar_style, 
                               bool is_vertical,
                               const value_range_format_t& format,
                               unsigned long min,
                               unsigned long max,
                               theme_t theme);
   
    void        measure(extents_t& result);
    void        place(const place_data_t& place_data);

    void        display(const any_regular_t& value);

    void        initialize(HWND parent);                         

    void        set_min_value(long min_value);
    void        set_max_value(long max_value);

    HWND                        control_m;

private:
    pb_style_t                  bar_style_m;
    bool                        is_vertical_m;

    double                      last_m;
    double                      value_m;
    value_range_format_t format_m;
    theme_t              theme_m;
};

/****************************************************************************************************/

}

/****************************************************************************************************/

#endif
