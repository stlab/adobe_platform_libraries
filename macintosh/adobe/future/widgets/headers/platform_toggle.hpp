/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_TOGGLE_HPP
#define ADOBE_WIDGET_TOGGLE_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/button_helper.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/keyboard.hpp>

#include <boost/gil/gil_all.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct toggle_t;

/****************************************************************************************************/

struct toggle_hit_handler_t
{
    explicit toggle_hit_handler_t(toggle_t* toggle) :
        toggle_m(toggle)
    {
        handler_m.insert(kEventClassControl, kEventControlHit);

        handler_m.monitor_proc_m = boost::bind(&toggle_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t handler_m;
    toggle_t*       toggle_m;
};

#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief toggle widget

    \model_of
        - \ref concept_placeable
        - \ref concept_view
        - \ref concept_controller
*/

struct toggle_t
{
    /*! The model type for this widget */
    typedef any_regular_t model_type;

    /*! Setter type for this widget */
    typedef boost::function<void (const model_type&)> setter_type;

    /*! Image type for this widget */
    typedef boost::gil::rgba8_image_t image_type;

    /*!
        Constructor.
    */
    toggle_t(const std::string&  alt_text,
             const any_regular_t value_on,
             const image_type&   image_on,
             const image_type&   image_off,
             const image_type&   image_disabled,
             theme_t             theme);

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const setter_type& proc);

    void enable(bool make_enabled);
    ///@}

    /*!
        @name View Concept Operations
        @{

        See the \ref concept_view concept and \ref view.hpp for more information.
    */
    /*!
        \note
            When the new value is set for the widget it will be compared to
            the show-value specified when the widget was created. If
            the new value matches the show-value, the widget will be rendered
            in its 'show' state. Otherwise, the widget will be rendered
            in its 'hide' state.
    */
    void display(const any_regular_t& to_value);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    toggle_hit_handler_t       hit_handler_m;
    std::string                alt_text_m;
    image_type                 image_on_m;
    image_type                 image_off_m;
    image_type                 image_disabled_m;
    setter_type                setter_proc_m;
    any_regular_t              value_on_m;
    any_regular_t              last_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
