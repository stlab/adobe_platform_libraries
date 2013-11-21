/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIDGET_PRESETS_HPP
#define ADOBE_WIDGET_PRESETS_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/platform_popup.hpp>

#include <adobe/eve.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/function.hpp>
#include <boost/operators.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
class sheet_t;
class assemblage_t;

struct presets_t;

/****************************************************************************************************/

typedef boost::function<dictionary_t ()> model_snapshot_proc_t;
typedef popup_t::setter_type                    preset_setter_type;

/****************************************************************************************************/

struct presets_hit_handler_t
{
    explicit presets_hit_handler_t(presets_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlClick);

        handler_m.monitor_proc_m = boost::bind(&presets_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t    handler_m;
    preset_setter_type setter_m;
    presets_t&         widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Presets widget

    \model_of
        - \ref concept_placeable

    The semantics of a preset widget is a widget that can save and restore "snapshots" of the state
    of the current model. These snapshots are given a name and can be recalled and imposed upon a
    model at any time, allowing for the user of the model to preserve, well, presets.
*/
struct presets_t
{
    /// model type for this widget
    typedef dictionary_t model_type;

    /// controller callback proc type for this widget
    typedef preset_setter_type setter_type;

    /*!
        \param name             Preset file name (xyz becomes xyz.preset)
        \param domain           Domain of the widget (usually the name of the application)
        \param alt_text         Additional help text for the widget when the user pauses over it
        \param bind_set         Array of name/string pairs that area values which will be saved/restored by presets
        \param localization_set Set of named arguments that contain the basic string keys for
                                localization of widget-private resources (e.g., the keys used
                                in the add/delete subdialogs to the preset widget).
        \param theme            Theme for the widget

        \note
            If the presets is set to a value other than <code>true_value</code> or
            <code>false_value</code>, the presets gets a 'dash' (undefined) state.
    */
    presets_t(const std::string&         name,
              const std::string&         domain,
              const std::string&         alt_text,
              const array_t&      bind_set,
              const dictionary_t& localization_set,
              theme_t                    theme);

#ifndef ADOBE_NO_DOCUMENTATION
    presets_t(const presets_t& rhs);

    presets_t& operator=(const presets_t& rhs);
#endif

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
        \note This function is to allow for the preset widget to obtain runtime-generated
              presets from the application (or property model) as is required by the app.

        \param value Set of additional preset data to be added to the preset listings
    */
    void display_additional_preset_set(const array_t& value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const setter_type& proc)
    { proc_m = proc; }
    ///@}

    /*!
        \param proc  Procedure called when a model snapshot is needed
    */
    void snapshot_callback(const model_snapshot_proc_t& proc)
    { snapshot_proc_m = proc; }

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;        // the preset icon
    popup_t                    category_popup_m; // the category popup
    popup_t                    popup_m;          // the actual preset popup
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    presets_hit_handler_t      hit_handler_m;    // hit handler for the preset icon
    array_t             bind_set_m;       // the set of parameters to be stored in the preset
    std::string                name_m;
    std::string                domain_m;
    std::string                alt_text_m;
    bool                       selected_m;       // whether or not the preset icon is pushed
    model_snapshot_proc_t      snapshot_proc_m;
    long                       popup_height_m;
    dictionary_t        localization_set_m;
    dictionary_t        last_m;
    bool                       type_2_debounce_m;
    setter_type                proc_m;
    bool                       custom_m;

    // these store the various preset sources as arrays
    array_t             dynamic_preset_set_m;
    array_t             default_preset_set_m;
    array_t             user_preset_set_m;
    array_t             composite_m;

    void do_imbue(const popup_t::model_type& value);

private:
    void display_custom();
#endif
};

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
inline ::HIViewRef get_display(presets_t& widget)
    { return widget.control_m; }
#endif
/****************************************************************************************************/

/*!
    \relates presets_t

    @name Controller Concept Operations
    @{

    See the \ref concept_controller concept and \ref controller.hpp for more information.
*/
void enable(presets_t& value, bool make_enabled);
///@}

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
bool operator==(const presets_t& x, const presets_t& y);
#endif
/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
