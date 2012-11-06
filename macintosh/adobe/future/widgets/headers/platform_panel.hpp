/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_PANEL_HPP
#define ADOBE_WIDGET_PANEL_HPP

/****************************************************************************************************/

#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Panel container widget

    \model_of
        - \ref concept_placeable
*/
struct panel_t
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /// constructor for this widget
    panel_t(const any_regular_t& show_value,
            theme_t                     theme);

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

    /*!
        @name View Concept Operations
        @{

        See the \ref concept_view concept and \ref view.hpp for more information.
    */
    void display(const any_regular_t& value);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    void set_visible(bool visible);

    ::ControlRef               control_m;
    any_regular_t       show_value_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
