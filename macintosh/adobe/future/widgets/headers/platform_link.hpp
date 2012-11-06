/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_LINK_HPP
#define ADOBE_WIDGET_LINK_HPP

/****************************************************************************************************/

#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct link_t;

/****************************************************************************************************/

typedef boost::function<void (const any_regular_t&)> link_setter_type;

/****************************************************************************************************/

struct link_hit_handler_t
{
    explicit link_hit_handler_t(link_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlClick);

        handler_m.monitor_proc_m = boost::bind(&link_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t  handler_m;
    link_setter_type setter_m;
    link_t&          widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Link widget

    \model_of
        - \ref concept_placeable
*/
struct link_t
{
    /// model typef for this widget
    typedef any_regular_t model_type;

    /// controller callback proc type for this widget
    typedef boost::function<void (const model_type&)> setter_type;

#ifndef ADOBE_NO_DOCUMENTATION
    BOOST_STATIC_ASSERT((boost::is_same<setter_type, link_setter_type>::value));
#endif

    /// constructor for this widget
    link_t(const std::string&          alt_text,
           const any_regular_t& on_value,
           const any_regular_t& off_value,
           long                        count,
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
    void display(const any_regular_t& to_value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void enable(bool make_enabled);

    void monitor(const setter_type& proc);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;
    std::string                alt_text_m;
    any_regular_t       on_value_m;
    any_regular_t       off_value_m;
    long                       count_m;
    theme_t                    theme_m;
    link_hit_handler_t         hit_handler_m;
    mutable metric_extractor_t metrics_m;
    guide_set_t                prongs_m;
    any_regular_t       value_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
