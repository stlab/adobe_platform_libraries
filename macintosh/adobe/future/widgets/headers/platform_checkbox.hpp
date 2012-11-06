/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_CHECKBOX_HPP
#define ADOBE_WIDGET_CHECKBOX_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>

#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/future/macintosh_events.hpp>

#include <boost/function.hpp>
#include <boost/operators.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct checkbox_t;

/****************************************************************************************************/

typedef boost::function<void (const any_regular_t&)> checkbox_setter_type;

/****************************************************************************************************/

struct checkbox_hit_handler_t
{
    explicit checkbox_hit_handler_t(checkbox_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlHit);

        handler_m.monitor_proc_m = boost::bind(&checkbox_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next, ::EventRef event);

    event_handler_t      handler_m;
    checkbox_setter_type setter_m;
    checkbox_t&          widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Checkbox widget

    \model_of
        - \ref concept_controller
        - \ref concept_placeable
        - \ref concept_view

    The semantics of a checkbox are that of a flag. One would use a checkbox when you
    want to have a selectable option turned on or off. There is a third state the
    checkbox can be in, the undetermined state, which is neither true or false. This
    is often represented with a dash or a filled checkbox.
*/

struct checkbox_t
{
    /*! The model type for this widget */
    typedef any_regular_t                      model_type;

    /*! The controller callback proc type for this widget */
    typedef boost::function<void (const model_type&)> setter_type;

#ifndef ADOBE_NO_DOCUMENTATION
    BOOST_STATIC_ASSERT((boost::is_same<setter_type, checkbox_setter_type>::value));
#endif

    /*!
        \param name is the name of the widget
        \param true_value is the value to which the checkbox is set when it is checked
        \param false_value is the value to which the checkbox is set when it is unchecked
        \param theme details additional visual attributes for the widget
        \param alt_text provides additional help text for the widget when the user pauses over it

        \note
            If the checkbox is set to a value other than <code>true_value</code> or
            <code>false_value</code>, the checkbox gets a 'dash' (undefined) state.
    */
    checkbox_t( const std::string&          name,
                const any_regular_t& true_value,
                const any_regular_t& false_value,
                theme_t                     theme,
                const std::string&          alt_text);

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
            When the new value is set for the checkbox_t it will be compared to
            the true- and false-values specified when the widget was created. If
            the new value matches the true-value, the checkbox will be checked. If
            the new value matches the false-value, the checkbox will not be checked.
            Otherwise, the checkbox will be set to an undetermined state.
    */
    void display(const any_regular_t& value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void enable(bool make_enabled);

    /*!
        \note
            The following table outlines the state changes for a checkbox_t when
            it is clicked on:
            <table>
                <tr><th>Previous value</th><th>New value</th></tr>
                <tr><td><code>false-value</code></td><td><code>true-value</code></td></tr>
                <tr><td><code>true-value</code></td><td><code>false-value</code></td></tr>
                <tr><td><code>other value</code></td><td><code>true-value</code></td></tr>
            </table>
    */
    void monitor(setter_type proc);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    friend bool operator==(const checkbox_t& x, const checkbox_t& y);

    ::ControlRef               control_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    checkbox_hit_handler_t     hit_handler_m;
    any_regular_t       true_value_m;
    any_regular_t       false_value_m;
    std::string                alt_text_m;
    std::string                name_m;
#endif
};

/****************************************************************************************************/

#ifndef ADOBE_NO_DOCUMENTATION
bool operator==(const checkbox_t& x, const checkbox_t& y);
#endif

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
