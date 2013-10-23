/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_TAB_GROUP_HPP
#define ADOBE_TAB_GROUP_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <vector>

#include <boost/utility.hpp>
#include <boost/function.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
typedef boost::function<void (const any_regular_t&)> tab_group_value_proc_t;
 
struct tab_group_t; 
struct tab_group_value_handler_t
{
    explicit tab_group_value_handler_t(tab_group_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlValueFieldChanged);

        handler_m.monitor_proc_m = boost::bind(&tab_group_value_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t        handler_m;
    tab_group_value_proc_t setter_m;
    tab_group_t&           widget_m;
};
#endif
/****************************************************************************************************/
/*!
    \ingroup apl_widgets_carbon

    \brief Tab group container widget
*/
struct tab_group_t : boost::noncopyable
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /// \todo remove me
    typedef tab_group_t  widget_type_t;

    /// tab structure
    struct tab_t
    {
        /// name of this tab
        std::string name_m;
        /// value associated with this tab being selected
        any_regular_t value_m;
    };

    /// constructor for the tab group widget. Takes an array of tab_t and a theme.
    tab_group_t(const tab_t* first, const tab_t* last, theme_t theme);

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
    void display(const any_regular_t& new_value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const tab_group_value_proc_t& proc);

    void enable(bool make_active)
        { implementation::set_active(control_m, make_active); }
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    typedef std::vector<tab_t> tab_set_t;

    ::ControlRef                control_m;
    theme_t                     theme_m;
    mutable metric_extractor_t  metrics_m;
    tab_set_t                   items_m;
    tab_group_value_handler_t   value_handler_m;
    bool                        debounce_m;
#endif
};

/****************************************************************************************************/

} //namespace adobe

/****************************************************************************************************/

#endif 

/****************************************************************************************************/
