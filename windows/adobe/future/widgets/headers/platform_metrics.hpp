/*
    Copyright 2006-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

// This header defines functions which can return various widget metrics
// on Windows systems. When available, the UXTHEME library is used to
// discover metrics. When UXTHEME is not available some reasonable
// defaults (precalculated on a system without UXTHEME) are returned.

/****************************************************************************************************/

#ifndef ADOBE_METRICS_HPP
#define ADOBE_METRICS_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>

#include <adobe/extents.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <uxtheme.h>
#include <tmschema.h>

#include <string>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/// The adobe::metrics suite of functions can fetch information
/// on ideal widget sizes. The design of this suite is to provide
/// simple metric and text information for a particular widget, and
/// require clients of the function suite to compose the metrics
/// information in a way that is meaningful for them.
///
/// All of the functions which return metrics require the widget type
/// and state of the widget. A table of all the part and state names
/// is available in the "Parts and States" documentation here:
/// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/userex/topics/partsandstates.asp

/****************************************************************************************************/

namespace metrics {

/****************************************************************************************************/

//
/// Measure text (given a window and platform placeable type) and return the optimal extents. See the
/// "Parts and States" documentation for more information on the platform placeable type. Takes
/// the text passed in lieu of the name of the window into consideration when measuring.
//

extents_t measure_text(const std::string& text, HWND window, int platform_placeable_type);

/****************************************************************************************************/

//
/// Measure the window (given a platform placeable type) and return the optimal extents. See the
/// "Parts and States" documentation for more information on the platform placeable type. Takes
/// the name of the window into consideration when measuring.
//

extents_t measure(HWND window, int platform_placeable_type);

/****************************************************************************************************/

//
/// This function uses the given metrics to calculate the optimal bounds of
/// a widget. This code isn't in metrics::measure because some controls
/// need to do a little more work to get the correct metrics (e.g.: popups
/// have a list of text which it needs space for, so it will have to get the
/// text extents for all of its items and pass the longest width extent to
/// this function).
///
/// If any of the arguments which this function takes are unknown (i.e.: metrics_t
/// wouldn't return a value) then pass NULL.
///
/// \param  widget_size the size of the widget, reported by metrics_t::get_size.
/// \param  widget_margins  the margins of the widget, reported by metrics_t::get_margins.
/// \param  font_metrics    the metrics of the font used by this widget, reported by metrics_t::get_font_metrics.
/// \param  text_extents    the extents of the text in the widget, reported by metrics_t::get_text_extents.
/// \param  border          the border of the widget, reported by metrics_t::get_integer.
///
/// \return the optimal bounds for this widget, based on the given information.
//

extents_t compose_measurements(const SIZE*       widget_size,
                                      const MARGINS*    widget_margins,
                                      const TEXTMETRIC* font_metrics,
                                      const RECT*       text_extents,
                                      const int*        border);

/****************************************************************************************************/

/// Get the correct font to use for the given widget.  This font must
/// be used for the font metrics to be correct.
///
/// \param  widget_type the type of the widget.
/// \param  out_font    a reference filled with the requested font.
///
/// \return true if the font was returned successfully, false if
///     the font could not be returned.

bool get_font(int widget_type, LOGFONTW& out_font);

/// Get the metrics for the font which should be used in the given widget.
///
/// \param  widget_type the type of the widget.
/// \param  out_metrics a reference filled with the text metrics.
///
/// \return true if the font metrics were returned successfully, false
///     if the metrics could not be returned.

bool get_font_metrics(int widget_type, TEXTMETRIC& out_metrics);

/// Get the extents for the given text string, to be contained in the
/// specified widget.
///
/// \param  widget_type the type of the widget.
/// \param  text        the string to return the extents for.
/// \param  out_extents a reference filled with the text extents.
///
/// \return true if the text extents were returned successfully, false
///     if the metrics could not be returned.

bool get_text_extents(int widget_type, std::wstring text, RECT& out_extents, const RECT* in_extents = NULL);

/// Get the specified measurement for the given widget when in the
/// given state.
///
/// \param  widget_type the type of the widget.
/// \param  measurement the required measurement.
/// \param  out_val     a reference filled with the requested value.
///
/// \return true if the value was returned successfully, false
///     if the value could not be found.

bool get_integer(int widget_type, int measurement, int& out_val);

/// Get the size of the specified widget.
///
/// \param  widget_type the type of the widget.
/// \param  measurement the size to get: TS_MIN, TS_TRUE, or TS_DRAW.
/// \param  out_size    a reference filled with the requested rectangle.
///
/// \return true if the rectangle was returned successfully, false
///     if the rectangle could not be found.

bool get_size(int widget_type, THEMESIZE measurement, SIZE& out_size);

/// Get the margins for the specified widget. Typically the margins
/// describe space which must be added around the text rectangle for
/// controls with a caption.
///
/// \param  widget_type the type of the widget.
/// \param  out_margins a reference filled with the margins of the widget.
///
/// \return true if the margins were returned successfully, false
///     if the margins could not be found.

bool get_margins(int widget_type, MARGINS& out_margins);

/// Get the text margins for a button widget. This call is specific
/// to a button widget, but is only available with Visual Styles on
/// Windows XP (hence is grouped in with the other metrics functions
/// of similar availability).
///
/// \param  widget_type the UXTheme type of button
/// \param  out_margins a rectangle containing the margins
///
/// \return true if the margins were returned successfully, false
///     if the margins could not be found.

bool get_button_text_margins(int widget_type, RECT& out_margins);

/// Return true if visual styles are currently in use, false if they
/// are not being used.
///
/// \return true if visual styles are in use, false otherwise.

bool using_styles();

/// Use the current style to draw the background of the parent control
/// of the given window using the given DC. This function is useful for
/// drawing the background of controls which appear on top of tab controls,
/// or other places where the color isn't regulation.
///
/// \param  window  the window to draw the parent background of.
/// \param  dc  the DC to draw with. This DC does not have to be
///         drawing onto the given window.

void draw_parent_background(HWND window, HDC dc);

/// Before any of the other functions can be called, the theme data must be
/// loaded from the window. This function should also be called any time the
/// window recieves a WM_THEMECHANGED message. Alternatively, set_theme_name
/// can be used.
///
/// \param  window  the window to get the theme from. Note that it is not
///         important which window is used, it does not have to
///         be the same window every time.
///
/// \return true if the theme was obtained from the window, false if the
///     theme could not be obtained from the window.

bool set_window(HWND window);

/// Before any of the other functions can be called, the theme name must be set.
/// Alternatively, set_window can be used.
///
/// \param  the name of the theme to use. 

void set_theme_name(const WCHAR* theme_name);

/****************************************************************************************************/

} // namespace metrics

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
