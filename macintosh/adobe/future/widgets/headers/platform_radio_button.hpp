/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_RADIO_BUTTON_HPP
#define ADOBE_WIDGET_RADIO_BUTTON_HPP

/****************************************************************************************************/

#include <adobe/any_regular.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct radio_button_t;

/****************************************************************************************************/

typedef boost::function<void (const any_regular_t&)> radio_button_monitor_proc_t;

/****************************************************************************************************/

struct radio_button_hit_handler_t
{
    explicit radio_button_hit_handler_t(radio_button_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlHit);

        handler_m.monitor_proc_m = boost::bind(&radio_button_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t             handler_m;
    radio_button_monitor_proc_t setter_m;
    radio_button_t&             widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Radio button widget

    The semantics of a radio button are that of a set of mutually exclusive options from 
    which the user must pick. One radio button is presented for every option available.
*/
struct radio_button_t
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /*!
        \param name     Name of the widget
        \param alt_text Alt text for the widget (supplemental information for the user)
        \param value    Value to which the radio button is set when it is selected
        \param theme    Theme for the widget
    */
    radio_button_t(const std::string&          name,
                   const std::string&          alt_text,
                   const any_regular_t& value,
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
    /*!
        \note
            When the new value is set for the radio button it will be compared to
            the value specified when the widget was created. If the new value matches
            that value the radio button will be selected. Otherwise, the radio button 
            will not be selected.
    */
    void display(const model_type& value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    /*!
        \note
            The following table outlines the state changes for a radio button when
            it is clicked on:
            <table>
                <tr><th>Previous state</th><th>New state</th></tr>
                <tr><td><code>not selected</code></td><td><code>selected</code></td></tr>
                <tr><td><code>selected</code></td><td><code>selected</code></td></tr>
            </table>
    */
    void monitor(const radio_button_monitor_proc_t& proc);

    /*!
        \param make_enabled Specifies whether or not to show this widget enabled
    */
    void enable(bool make_enabled);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;
    std::string                name_m;
    std::string                alt_text_m;
    any_regular_t       set_value_m;
    theme_t                    theme_m;
    radio_button_hit_handler_t hit_handler_m;
    any_regular_t       last_m;
    mutable metric_extractor_t metrics_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
