/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_OPTIONAL_PANEL_HPP
#define ADOBE_WIDGET_OPTIONAL_PANEL_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <adobe/future/widgets/headers/platform_panel.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Optional panel container widget

    \model_of
        - \ref concept_placeable
*/
struct optional_panel_t : boost::noncopyable
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /// optional display proc type. Used in combination with functions in the widget
    /// factory to correctly call the appropriate function during show/hide
    typedef boost::function<void (const boost::function<void ()>&)> optional_display_proc_t;

    /// constructor for this widget
    optional_panel_t(const any_regular_t& show_value,
                     theme_t              theme);

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
    void set_optional_display_procs(const optional_display_proc_t& show_proc,
                                    const optional_display_proc_t& hide_proc)
    {
        show_proc_m = show_proc;
        hide_proc_m = hide_proc;
    }

    panel_t                 control_m;
    optional_display_proc_t show_proc_m;
    optional_display_proc_t hide_proc_m;
    bool                    inited_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
