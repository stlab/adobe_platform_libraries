/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_SLIDER_HPP
#define ADOBE_WIDGET_SLIDER_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/extents.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/slider_helper.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/memory.hpp>
#include <adobe/name.hpp>

#include <boost/static_assert.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
typedef boost::function<void (const long&)> slider_setter_type;
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Slider widget
*/
struct slider_t
{
    /// model type for the widget
    typedef long model_type;

    /// controller callback proc type for the widget
    typedef boost::function<void (const model_type&)> setter_type;

#ifndef ADOBE_NO_DOCUMENTATION
    BOOST_STATIC_ASSERT((boost::is_same<setter_type, slider_setter_type>::value));
#endif

    /*!
        \param alt_text         Additional help text for the widget when the user pauses over it
        \param is_vertical      Toggle the vertical orientation of the slider
        \param style            Denotes style attributes for the slider
        \param num_ticks        Number of tick marks for the slider
        \param format           Formatter used to filter values for the slider
        \param theme            Theme for the widget
    */
    slider_t(const std::string&          alt_text,
             bool                        is_vertical,
             slider_style_t              style,
             std::size_t                 num_ticks,
             const value_range_format_t& format,
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
    void display(const model_type& value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const setter_type& proc);

    void enable(bool make_enabled);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;
    mutable metric_extractor_t metrics_m;
    std::string                alt_text_m;
    bool                       is_vertical_m;
    slider_style_t             style_m;
    std::size_t                num_ticks_m;
    value_range_format_t       format_m;
    theme_t                    theme_m;
    setter_type                value_proc_m;
    long                       last_m; // debounce
    long                       value_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
