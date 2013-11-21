/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIDGET_LABEL_T_HPP
#define ADOBE_WIDGET_LABEL_T_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>

#include <adobe/eve.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
class sheet_t;
class assemblage_t;

/****************************************************************************************************/

enum label_subevent_t
{
    subevent_none_s = 0,
    subevent_mouse_down_s,
    subevent_mouse_up_s,
    subevent_mouse_in_s,
    subevent_mouse_out_s
};

typedef boost::function<void (label_subevent_t)> label_subevent_callback_t;

struct label_subevent_handler_t
{
    label_subevent_handler_t() :
        control_m(0)
    {
        handler_m.insert(kEventClassControl, kEventControlClick);
        handler_m.insert(kEventClassControl, kEventControlHit);
        handler_m.insert(kEventClassControl, kEventControlTrackingAreaEntered);
        handler_m.insert(kEventClassControl, kEventControlTrackingAreaExited);

        handler_m.monitor_proc_m = boost::bind(&label_subevent_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t           handler_m;
    label_subevent_callback_t callback_m;
    ::ControlRef              control_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Label widget

    \model_of
        - \ref concept_placeable
*/
struct label_t : boost::equality_comparable<label_t>
{
    /// model type for this widget
    typedef std::string model_type;

    /// constructor for this widget
    label_t(const std::string& name,
            const std::string& alt_text,
            std::size_t        characters,
            theme_t            theme);

#ifndef ADOBE_NO_DOCUMENTATION
    auto_control_t             control_m;
    std::string                name_m;
    std::string                alt_text_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    std::size_t                characters_m;
    label_subevent_handler_t   subevent_m;
#endif
};

/****************************************************************************************************/

/*!
    \relates label_t

    @name PlaceableTwopass Concept Operations
    @{

    See the \ref concept_placeable_twopass concept and \ref placeable.hpp for more information.
*/
void measure(label_t& value, extents_t& result);

void measure_vertical(label_t& value, extents_t& calculated_horizontal, 
                        const place_data_t& placed_horizontal);

void place(label_t& value, const place_data_t& place_data);
///@}

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION

/****************************************************************************************************/

void enable(label_t& value, bool make_enabled);
bool operator==(const label_t& x, const label_t& y);

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

template <>
std::string get_name<label_t>(const label_t& widget);

/****************************************************************************************************/

template <>
void set_name< label_t >(label_t& control, const std::string& name);
#endif
/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
void measure_label_text(const std::string& text, theme_t theme, extents_t& result);
#endif
/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
