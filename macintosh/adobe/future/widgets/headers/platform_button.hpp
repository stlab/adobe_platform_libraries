/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_BUTTON_HPP
#define ADOBE_WIDGET_BUTTON_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/button_helper.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/keyboard.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct button_t;

/****************************************************************************************************/

struct button_hit_handler_t
{
    explicit button_hit_handler_t(button_t* button) :
        button_m(button)
    {
        handler_m.insert(kEventClassControl, kEventControlHit);

        handler_m.monitor_proc_m = boost::bind(&button_hit_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t handler_m;
    button_t*       button_m;
};

/****************************************************************************************************/

struct button_modifier_key_handler_t
{
    explicit button_modifier_key_handler_t(button_t* button) :
        button_m(button)
    {
        handler_m.insert(kEventClassKeyboard, kEventRawKeyModifiersChanged);

        handler_m.monitor_proc_m = boost::bind(&button_modifier_key_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t handler_m;
    button_t*       button_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Button widget

    \model_of
        - \ref concept_placeable
        - Latch
        - KeyboardHandler

    The semantics of a button are that of a latch. It retains
    information about a cell or set of cells in the property model. The
    button also has one or more actions to which it is bound. The user
    selects which action they would like to employ by holding down
    various modifier keys (alt, ctrl, etc.) When the user clicks on a
    button, the retained state is sent along with the current action to
    a notifier callback, where it is processed.
*/

struct button_t
{
    /*! The model type for this widget */
    typedef any_regular_t model_type;

    /*!
        \param is_default denotes whether or not this button is the
        'default' button. On some platforms this has specific visual
        (e.g., pulsing) and behavioral (e.g., firing when the RETURN key
        is pressed) attributes.

        \param is_cancel denotes whether or not this button is the
        default 'cancel' button. On some platforms this has specific
        behavioral attributes (e.g., firing when the user presses the
        ESC key).

        \param modifier_mask represents a mask of all the possible
        modifier key states by which this button may change.

        \param first is the first button state descriptor

        \param last is one-past-the-last button state descriptor

        \param theme details additional visual attributes for the widget
    */
    button_t(bool                             is_default,
             bool                             is_cancel,
             modifiers_t               modifier_mask,
             const button_state_descriptor_t* first,
             const button_state_descriptor_t* last,
             theme_t                   theme);

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
    /*!
        \note (fbrereto) : this should probably take place on a per-state basis.
    */
    void enable(bool make_enabled);
    ///@}

    /*!
        \param modifiers specifies to which state this value will apply
        \param value is the information to retain for this sublatch in the button
    */
    void set(modifiers_t modifiers, const model_type& value);

    /*!
        \param modifiers specifies to which state this contributing set will apply
        \param value is the contributing set information for this sublatch in the button
    */
    void set_contributing(modifiers_t modifiers, const dictionary_t& value);

    /*!
        @name KeyboardHandler Concept Operations
        @{
    */
    /*!
        \return the platform-specific handler for this widget that
        models the requirements of a keyboard event handler.
    */
    any_regular_t underlying_handler() { return any_regular_t(control_m); }

    /*!
        \param key is the relevant key for this keyboard event
        \param pressed denotes whether this event is key-down or key-up
        \param modifiers is an bitflag set denoting the currently-depressed modifier keys

        \return \c true if this widget consumed the event; \c false otherwise.
    */
    bool handle_key(key_type key, bool pressed, modifiers_t modifiers);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef                    control_m;
    theme_t                         theme_m;
    mutable metric_extractor_t      metrics_m;
    button_state_set_t              state_set_m;
    button_hit_handler_t            hit_handler_m;
    button_modifier_key_handler_t   mod_key_handler_m;
    modifiers_t                     modifier_mask_m;
    bool                            is_default_m;
    bool                            is_cancel_m;
    bool                            enabled_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
