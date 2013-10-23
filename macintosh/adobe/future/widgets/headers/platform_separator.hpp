/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_SEPARATOR_HPP
#define ADOBE_SEPARATOR_HPP

/****************************************************************************************************/

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
/*!
    \ingroup apl_widgets_carbon

    \brief Separator widget
*/
struct separator_t : boost::noncopyable
{    
    /// constructor for this widget
    separator_t(bool is_vertical, theme_t theme);

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

    /// remove me
    void set_visible(bool make_visible);

#ifndef ADOBE_NO_DOCUMENTATION    
    ::ControlRef                control_m;
    mutable metric_extractor_t  metrics_m;
    bool            			is_vertical_m;
    theme_t  			        theme_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif
