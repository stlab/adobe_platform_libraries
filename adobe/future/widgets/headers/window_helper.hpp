/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WINDOW_HELPER_HPP
#define ADOBE_WINDOW_HELPER_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/enum_ops.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!

\ingroup ui_core

\brief Window style

\note
    These should probably be eliminated; in their place we should be able to
    specify the intention of the window, and the ui_core should discern the
    proper style for it automatically. (dialog_t v. palette_t, for example.)
*/

enum window_style_t
{
    /// Dialog-like; user cannot interface with other windows until
    /// this dialog is dismissed. However, the dialog can be freely
    /// moved about the screen
    window_style_moveable_modal_s,

    /// palette-like; smaller screen real estate consumption and an
    /// always-on-top behavior combine to reflect a dialog that can
    /// be interacted with at any time to edit settings of the current
    /// application state
    window_style_floating_s
};

/****************************************************************************************************/

/*!
\ingroup ui_core

\brief Window modality

\note
    In eliminating the other enumerations (like window_reposition_t
    and window_attributes_t I would suspect this enumeration set to
    be the most likely candidate for the replacement enumeration;
    that is to say, these window behaviors would form the foundation
    for window positions and attributes. Even still, modifications to
    this enumeration would be made, as we're talking about modality 
    here and not the semantics of the window at large. Also, the
    settings seem to be very Mac-specific, and so should be refactored
    to handle a larger audience of platforms.
*/

enum window_modality_t
{
    /// The window does not prevent the user from interacting with
    /// any other window
    window_modality_none_s,

    /// For the most part produces the same behavior as
    /// window_modality_app_s. In some instances this will force the
    /// user to dismiss this window before any other actions can take
    /// place on the system
    window_modality_system_s,

    /// This window takes top priority in the application; no other
    /// window can be interacted with until this window is dismissed
    window_modality_app_s,

    /// When this window is related to another window (a document, say)
    /// this setting prevents the user from interacting with that other
    /// window until this one is dismissed
    window_modality_window_s
};

/****************************************************************************************************/

/*!
\ingroup ui_core

\brief Window attributes

\note
    These should probably be eliminated; in their place we should be able to
    specify the intention of the window, and the ui_core should discern the
    proper attribute(s) for it automatically.
*/

enum window_attributes_t
{
    /// No attributes
    window_attributes_none_s                = 0,

    /// Events are handled by the OS (blech)
    window_attributes_standard_handler_s    = 1 << 0,

    /// Permits the window to be resizeable; also adds a grow box
    window_attributes_resizeable_s          = 1 << 1,

    /// Permits growing to be "live" -- that is, resizing results are instant
    window_attributes_live_resizeable_s     = 1 << 2,

    /// MacOS-specific (blech) flag; adds a metal appearance
    window_attributes_metal_s               = 1 << 3
};

/****************************************************************************************************/

/*!
\ingroup ui_core

\brief Window positions on the main screen

\note
    These should probably be eliminated; in their place we should be able to
    specify the intention of the window, and the ui_core should discern the
    proper placement location for it automatically.
*/

enum window_reposition_t
{
    /// Position the window centered vertically and horizontally on the screen
    window_reposition_center_s,

    /// Position the window in the "alert" position. Typically this is centered
    /// horizontally on the screen but closer to the top of the screen vertically.
    window_reposition_alert_s
};

/****************************************************************************************************/

typedef boost::function<void (long, long)> window_resize_proc_t;
typedef boost::function<bool ()>           window_close_proc_t;

/****************************************************************************************************/

ADOBE_DEFINE_BITSET_OPS(window_attributes_t)

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
