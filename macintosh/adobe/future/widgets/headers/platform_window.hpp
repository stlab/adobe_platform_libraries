/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_WINDOW_HPP
#define ADOBE_WIDGET_WINDOW_HPP

/****************************************************************************************************/

#include <adobe/eve.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/window_helper.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct window_t;

/****************************************************************************************************/

struct window_resize_handler_t
{
    explicit window_resize_handler_t(window_t& window) :
        window_m(window)
    {
        handler_m.insert(kEventClassWindow, kEventWindowBoundsChanged);

        handler_m.monitor_proc_m = boost::bind(&window_resize_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t      handler_m;
    window_resize_proc_t callback_m;
    window_t&            window_m;
};

/****************************************************************************************************/

struct window_close_handler_t
{
    explicit window_close_handler_t(window_t& window) :
        window_m(window)
    {
        handler_m.insert(kEventClassWindow, kEventWindowClose);

        handler_m.monitor_proc_m = boost::bind(&window_close_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t      handler_m;
    window_close_proc_t callback_m;
    window_t&           window_m;
};
#endif
/****************************************************************************************************/
/*!
    \ingroup apl_widgets_carbon

    \brief Window container widget
*/
struct window_t
{
    /// constructor for this widget
    window_t(const std::string&  name,
             window_style_t      style,
             window_attributes_t attributes,
             window_modality_t   modality,
             theme_t             theme);

    /// destrcutor for this widget
    ~window_t();

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    void set_visible(bool make_visible);

    void set_size(const point_2d_t& size);

    void reposition(window_reposition_t position);
 
    void monitor_resize(const window_resize_proc_t& proc);

    any_regular_t underlying_handler() { return any_regular_t(window_m); }

    bool handle_key(key_type /*key*/, bool /*pressed*/, modifiers_t /*modifiers*/)
        { return false; }

    ::WindowRef                window_m;
    ::ControlRef               root_control_m;
    std::string                name_m;
    window_style_t             style_m;
    window_attributes_t        attributes_m;
    window_modality_t          modality_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    point_2d_t                 dimensions_m;
    bool                       debounce_m; // for resizing transition
    window_resize_handler_t    resize_handler_m;
    window_close_handler_t     close_handler_m;
    bool                       placed_once_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
