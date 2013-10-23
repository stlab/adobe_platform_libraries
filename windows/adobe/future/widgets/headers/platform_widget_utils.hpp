/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_UI_CORE_OS_UTILITIES_HPP
#define ADOBE_UI_CORE_OS_UTILITIES_HPP

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>

#include <adobe/config.hpp>

#include <adobe/future/windows_cast.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/name_fwd.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

/****************************************************************************************************/

namespace hackery {

/****************************************************************************************************/

std::string convert_utf(const WCHAR* buffer, std::size_t size);
std::string convert_utf(const WCHAR* buffer);

#ifndef ADOBE_PLATFORM_CYGWIN
    std::wstring convert_utf(const CHAR* buffer, std::size_t size);
    std::wstring convert_utf(const CHAR* buffer);

    inline std::wstring convert_utf(const std::string& name)
    { return convert_utf(name.c_str(), name.size()); }
#endif

/****************************************************************************************************/

} // namespace hackery

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

typedef HWND platform_control_type;

/****************************************************************************************************/

std::string get_window_title(HWND window);

inline std::string get_control_string(HWND control)
{ return implementation::get_window_title(control); }

void get_control_bounds(HWND control, RECT& bounds);

void set_control_bounds(HWND control, const place_data_t& place_data);

template <typename T>
inline std::string get_field_text(T& x)
{ return get_control_string(x.control_m); }

template <>
inline std::string get_field_text<HWND>(HWND& x)
{ return get_control_string(x); }

/****************************************************************************************************/

template <typename T>
inline bool is_focused(T& control)
{ return is_focused(control.control_m); }

template <>
inline bool is_focused(HWND& control)
{ return ::GetFocus() == control; }

/****************************************************************************************************/

void throw_last_error_exception(const char* file, long line);

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

//
/// Win32's event mechanism is tweaked in that sometimes a parent of a widget is called when the user
/// changes something about the widget. forward_message is intended to let the pertinent window have
/// a stab at the event. We want to use the window's standard message handling system so we don't have
/// to introduce any new event hierarchies.
///
/// /return true if the forward_result should be used as a return value; false otherwise.
//

bool forward_message(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& forward_result);

/****************************************************************************************************/

//
/// Information on the uxtheme_type parameter's values is
/// available in the "Parts and States" documentation here:
/// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/userex/topics/partsandstates.asp
//

void set_font(HWND window, int uxtheme_type);

/****************************************************************************************************/

inline void set_control_visible(HWND control, bool make_visible)
{
    assert(control);

    ::ShowWindow(control, make_visible ? SW_SHOWNORMAL : SW_HIDE);
}

/****************************************************************************************************/

inline void set_control_enabled(HWND control, bool make_enabled)
{
    assert(control);

    ::EnableWindow(control, make_enabled);
}

/****************************************************************************************************/

bool context_menu(HWND parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result);

/****************************************************************************************************/

LONG_PTR get_user_reference(HWND control);

/****************************************************************************************************/

template <typename T>
void set_user_reference(HWND control, T data)
{
    assert(control);

    ::SetWindowLongPtr(control, GWLP_USERDATA, hackery::cast<LONG>(data));
}

/****************************************************************************************************/

modifiers_t convert_modifiers(ULONG os_modifiers);
modifiers_t convert_modifiers(BYTE  keyboard_state[256]);

modifiers_t modifier_state();

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#ifndef ADOBE_THROW_LAST_ERROR
    #define ADOBE_THROW_LAST_ERROR adobe::implementation::throw_last_error_exception(__FILE__, __LINE__)
#endif

/****************************************************************************************************/

#endif

/****************************************************************************************************/
