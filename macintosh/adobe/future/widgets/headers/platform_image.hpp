/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_IMAGE_T_HPP
#define ADOBE_IMAGE_T_HPP

/****************************************************************************************************/

#include <boost/gil/image.hpp>

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>

#include <adobe/eve.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
class sheet_t;
class assemblage_t;
struct image_t;
#endif
/****************************************************************************************************/

struct image_event_handler_t
{
    explicit image_event_handler_t(image_t& widget);

    event_handler_t handler_m;
    image_t&        widget_m;
};

/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Image widget

    \model_of
        - \ref concept_placeable
        - \ref concept_view
*/
struct image_t
{
    /// model types for this widget
    typedef dictionary_t                                         controller_model_type;
    typedef boost::gil::rgba8_image_t                            view_model_type;
    typedef boost::function<void (const controller_model_type&)> setter_proc_type;

    /// constructor for this widget
    image_t(const view_model_type& image);

    /*!
        @name PlaceableTwopass Concept Operations
        @{
    
        See the \ref concept_placeable_twopass concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);
        
    void measure_vertical(extents_t& calculated_horizontal, const place_data_t& placed_horizontal);
    
    void place(const place_data_t& place_data);
    ///@}
    
    void display(const view_model_type& value);

    void monitor(const setter_proc_type& proc);
    void enable(bool make_enabled);

    ::OSStatus handle_event(::EventHandlerCallRef next, ::EventRef event);

#ifndef ADOBE_NO_DOCUMENTATION
    auto_control_t             control_m;
    mutable metric_extractor_t metrics_m;
    view_model_type            image_m;
    setter_proc_type           callback_m;
    bool                       enabled_m;
    image_event_handler_t      handler_m;
    dictionary_t               metadata_m;
    std::pair<long, long>      origin_m;
#endif
};

/****************************************************************************************************/

inline image_event_handler_t::image_event_handler_t(image_t& widget) :
    widget_m(widget)
{
    handler_m.insert(kEventClassControl, kEventControlClick);

    handler_m.monitor_proc_m = boost::bind(&image_t::handle_event, boost::ref(widget_m), _1, _2);
}

/****************************************************************************************************/

template <>
struct controller_model_type<image_t>
{ typedef image_t::controller_model_type type; };

template <>
struct view_model_type<image_t>
{ typedef image_t::view_model_type type; };

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
