/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_REVEAL_HPP
#define ADOBE_WIDGET_REVEAL_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>

#include <adobe/eve.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <boost/function.hpp>
#include <boost/operators.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
class sheet_t;
class assemblage_t;

struct reveal_t;

/****************************************************************************************************/

typedef boost::function<void (const any_regular_t&)> reveal_setter_type;

/****************************************************************************************************/

struct reveal_hit_handler_t
{
    explicit reveal_hit_handler_t(reveal_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlHit);

        handler_m.monitor_proc_m = boost::bind(&reveal_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t    handler_m;
    reveal_setter_type setter_m;
    reveal_t&          widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Reveal widget

    The semantics of a reveal are that of a ui element that controls the state of a visibility flag.
    One would use a reveal widget when there is a portion of a view that you want optionally
    viewable by the user, at the user's discretion (e.g., an "advanced" subsection of the dialog.)
*/
struct reveal_t
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /// controller callback proc type for this widget
    typedef boost::function<void (const model_type&)> setter_type;

#ifndef ADOBE_NO_DOCUMENTATION
    BOOST_STATIC_ASSERT((boost::is_same<setter_type, reveal_setter_type>::value));
#endif

    /*!
        \param name         Label for the widget
        \param show_value   Value to which the reveal is set when in its show state
        \param theme        Theme for the widget
        \param alt_text     optional text for tooltip display

        \note
            If the reveal is set to a value other than <code>show_value</code>, 
            the reveal is considered in its 'hide' state.
    */
    reveal_t(const std::string&     name,
             const any_regular_t&   show_value,
             theme_t                theme,
             const std::string&     alt_text);

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
            When the new value is set for the reveal_t it will be compared to
            the show-value specified when the widget was created. If
            the new value matches the show-value, the reveal will be rendered
            in its 'show' state. Otherwise, the reveal will be rendered
            in its 'hide' state.
    */
    void display(const any_regular_t& to_value);

    /*!
        \note
            This API is the similar to the set operation that takes an any_regular_t,
            and is intended as an easier way of manipulating a reveal that has
            default show-value.
    */
    inline void display(const bool& to_value)
        { display(any_regular_t(to_value)); }
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(setter_type proc);

    void enable(bool make_enabled);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    auto_control_t              control_m;
    theme_t                     theme_m;
    std::string                 name_m;
    mutable metric_extractor_t  metrics_m;
    reveal_hit_handler_t        hit_handler_m;
    any_regular_t               last_m;
    any_regular_t               show_value_m;
    std::string                 alt_text_m;
#endif
};

/****************************************************************************************************/

namespace view_implementation {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
inline void set_value_from_model(reveal_t& value, const any_regular_t& new_value)
{ value.display(new_value); }
#endif
/****************************************************************************************************/

} // namespace view_implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
