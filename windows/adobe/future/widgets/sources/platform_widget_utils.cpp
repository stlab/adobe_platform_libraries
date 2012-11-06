/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

//#define WIN32_LEAN_AND_MEAN 1

#include <windows.h>
#include <Commdlg.h>
#include <CommCtrl.h>

#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/name.hpp>
#include <adobe/unicode.hpp>

#include <boost/cstdint.hpp>

#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

HWND tooltip_control()
{
    /*
        Tooltips on Windows are windows in and of themselves, and they have a queue of tools to which
        they are bound. In order to take advantage of this we have one global tooltip window, and each
        tool is bound to it with its alternative text string.
    */

    static HWND tooltip_s = CreateWindowEx(WS_EX_TOPMOST,
                                           TOOLTIPS_CLASS,
                                           NULL,
                                           WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           NULL,
                                           NULL,
                                           ::GetModuleHandle(NULL),
                                           NULL
                                           );
    static bool inited(false);

    if (!inited)
    {
        SetWindowPos(tooltip_s,
                     HWND_TOPMOST,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        inited = true;
    }

    return tooltip_s;
}

/****************************************************************************************************/

std::string window_class(HWND window)
{
    if (::IsWindow(window))
    {
        std::vector<char> class_name(64, 0);

        int num_copied = ::GetClassNameA(window, &class_name[0], static_cast<int>(class_name.size()));

        return std::string(&class_name[0], num_copied);
    }

    return std::string();
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace hackery {

/****************************************************************************************************/

std::string convert_utf(const WCHAR* buffer, std::size_t size)
{
    assert(buffer);

    typedef     std::vector<char> buftype;
    int         length = WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(size), 0, 0, 0, 0);
    buftype     tmpbuf(length + 1);
    //
    // Convert characters.
    //
    WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(size), &(tmpbuf[0]), length, 0, 0);
    return std::string(&(tmpbuf[0]));
}

/****************************************************************************************************/

std::string convert_utf(const WCHAR* buffer)
{
    assert(buffer);

    std::size_t size(0);

    while (buffer[size] != WCHAR(0)) ++size;

    return convert_utf(buffer, size);
}

/****************************************************************************************************/

std::wstring convert_utf(const CHAR* buffer, std::size_t size)
{
    assert(buffer);

    typedef     std::vector<WCHAR> buftype;
    int         length = MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(size), 0, 0);
    buftype     tmpbuf(length + 1);
    //
    // Convert characters.
    //
    MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(size), &(tmpbuf[0]), length);
    return std::wstring(&(tmpbuf[0]), tmpbuf.size());
}

/****************************************************************************************************/

std::wstring convert_utf(const CHAR* buffer)
{
    assert(buffer);

    std::size_t size(0);

    while (buffer[size] != CHAR(0)) ++size;

    return convert_utf(buffer, size);
}

/****************************************************************************************************/

} // namespace hackery

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

boost::filesystem::path to_path(const std::vector<TCHAR>& path_buffer)
{
    // At this point we have a path to the save file location, but we don't know
    // if the return value is utf16 or utf8. Boost::filesystem at the moment requires
    // utf8 (or ASCII?) so we need to make the conversion at runtime if necessary.
    // That's what this code does.

    std::vector<char> cpath_buffer;

    const TCHAR* end = std::find(&path_buffer[0], &path_buffer[0] + path_buffer.size(), TCHAR(0));

    cpath_buffer.reserve(std::distance(&path_buffer[0], end));

    adobe::to_utf8(&path_buffer[0], end, std::back_inserter(cpath_buffer));

    cpath_buffer.push_back(0);

    // finally, construct the new path from the converted string buffer and return

    return boost::filesystem::path(&cpath_buffer[0], boost::filesystem::native);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

bool pick_file(boost::filesystem::path& path, platform_display_type dialog_parent)
{
    // This upper part sets up the OS-specific open file dialog
    // and invokes it so the user can pick a file to open

    std::vector<TCHAR> path_buffer(2048, 0);
    OPENFILENAME       params = { 0 };

    params.hwndOwner = dialog_parent;
    params.lStructSize = sizeof(params);
    params.lpstrFilter = L"Text File\0*.txt\0All Files\0*.*\0\0";
    params.lpstrFile = &path_buffer[0];
    params.nMaxFile = static_cast<DWORD>(path_buffer.size());
    params.Flags = OFN_DONTADDTORECENT |
                   OFN_LONGNAMES |
                   OFN_NOCHANGEDIR |
                   OFN_NONETWORKBUTTON |
                   OFN_PATHMUSTEXIST |
                   OFN_FILEMUSTEXIST;
    params.lpstrDefExt = L"txt";

    bool result(::GetOpenFileName(&params) != 0);

    if (result)
        path = to_path(path_buffer);

    return result;
}

/****************************************************************************************************/

bool pick_save_path(boost::filesystem::path& path, platform_display_type dialog_parent)
{
    // This upper part sets up the OS-specific save file dialog and
    // invokes it so the user can pick a save file location

    std::vector<TCHAR> path_buffer(2048, 0);
    OPENFILENAME       params = { 0 };

    params.hwndOwner = dialog_parent;
    params.lStructSize = sizeof(params);
    params.lpstrFilter = L"Text File\0*.txt\0All Files\0*.*\0\0";
    params.lpstrFile = &path_buffer[0];
    params.nMaxFile = static_cast<DWORD>(path_buffer.size());
    params.Flags = OFN_DONTADDTORECENT |
                   OFN_LONGNAMES |
                   OFN_NOCHANGEDIR |
                   OFN_NONETWORKBUTTON |
                   OFN_NOREADONLYRETURN |
                   OFN_PATHMUSTEXIST;
    params.lpstrDefExt = L"txt";

    bool result(::GetSaveFileName(&params) != 0);

    if (result)
        path = to_path(path_buffer);

    return result;
}

/****************************************************************************************************/

void set_control_alt_text(HWND control, const std::string& alt_text)
{
    HWND               tooltip(tooltip_control()); // handle to the ToolTip control
    TOOLINFO           tooltip_info;               // struct specifying info about tool in ToolTip control
    std::vector<TCHAR> text_buffer(alt_text.size() * 2, 0);

    /* Set up the text for filling in the tooltip structure below */
    to_utf16(&alt_text[0], &alt_text[0] + alt_text.size(), &text_buffer[0]);

    /* Basic initialization of the tooltip structure parameters */
    tooltip_info.cbSize = sizeof(TOOLINFO);
    tooltip_info.uFlags = TTF_SUBCLASS | TTF_IDISHWND | TTF_CENTERTIP;
    tooltip_info.hwnd = control;
    tooltip_info.hinst = ::GetModuleHandle(NULL);
    tooltip_info.uId = hackery::cast<UINT_PTR>(control);
    tooltip_info.lpszText = &text_buffer[0];

    // ToolTip control will cover the whole window
    ::GetClientRect (control, &tooltip_info.rect);

    /*
        First delete any previous references to this control from the TT's tool list. This
        allows for widgets with potentially multiple alt_texts (e.g., buttons and their
        different states) to display each one when appropriate. If there is no previous
        reference then this does nothing, which is fine.
    */
    SendMessage(tooltip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &tooltip_info);

    /* At the last, we bind the tooltip to the control */
    SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &tooltip_info);
}

/****************************************************************************************************/

std::string get_window_title(HWND window)
{
    assert(window);

    int const buffer_size = ::GetWindowTextLengthW(window) + 1;

    std::vector<WCHAR> titlename(buffer_size, 0);

    int const text_length = ::GetWindowTextW(window, &titlename[0], buffer_size);

    assert(text_length < buffer_size);

    return std::string(hackery::convert_utf(&titlename[0]));
}

/****************************************************************************************************/

void get_control_bounds(HWND control, RECT& bounds)
{
    assert(control);

    ::GetWindowRect(control, &bounds);
}

/****************************************************************************************************/

void set_control_bounds(HWND control, const place_data_t& place_data)
{
    assert(control);

    ::MoveWindow(control, left(place_data), top(place_data), width(place_data), height(place_data), TRUE);
}

/****************************************************************************************************/

void throw_last_error_exception(const char* /* file */, long /* line */)
{
    DWORD error(::GetLastError());
    char  the_message[2048] = { 0 };

    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        error,
        0,
        &the_message[0],
        sizeof(the_message),
        NULL);

    char* actual_error = the_message[0] ? the_message : "Unknown Error.";

    ::MessageBoxA(0, actual_error, "Error Message From Windows", MB_ICONEXCLAMATION | MB_APPLMODAL | MB_OK);

    throw std::runtime_error(the_message);
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

bool forward_message(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& forward_result)
{
    static const std::string listview_class_k("SysListView32");

    if (message == WM_COMMAND)
    {
        HWND actual_window(reinterpret_cast<HWND>(lParam));

        if (actual_window == NULL)
            return false;

        if (window_class(actual_window) == listview_class_k)
            return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        return forward_result == 0;
    }
    else if (message == WM_NOTIFY)
    {
        NMHDR& notify_header(*reinterpret_cast<NMHDR*>(lParam));
        HWND   actual_window(notify_header.hwndFrom);

        if (actual_window == NULL)
            return false;

        //if (window_class(actual_window) == listview_class_k)
        //    return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        // From WM_NOTIFY docs:
        // "The return value is ignored except for notification messages that specify otherwise."
        // Lame.

        // note: this list is not exhaustive. Items should be added to the list
        //       as notification messages are found that require return values.

        bool notify_message_set_that_returns_stuff(message == NM_CUSTOMDRAW);

        return notify_message_set_that_returns_stuff;
    }
    else if (message == WM_HSCROLL || message == WM_VSCROLL)
    {
        HWND actual_window(reinterpret_cast<HWND>(lParam));

        if (actual_window == NULL)
            return false;

        if (window_class(actual_window) == listview_class_k)
            return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        return forward_result == 0;
    }

    return false;
}

/****************************************************************************************************/

bool context_menu(HWND parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result)
{
    HMENU menu(::CreatePopupMenu());

    const name_t* the_first(first);

    long count(0);

    for (; first != last; ++first)
    {
        MENUITEMINFOA item = { 0 };
        std::size_t   length(std::strlen(first->c_str()));
        bool          is_separator(*first == static_name_t("-"));

        item.cbSize = sizeof(item);
        item.fMask = MIIM_FTYPE | MIIM_ID | (is_separator ? 0 : MIIM_STRING);
        item.fType = is_separator ? MFT_SEPARATOR : MFT_STRING;
        item.wID = ++count;
        item.dwTypeData = (LPSTR)first->c_str();
        item.cch = (UINT)length;

        ::InsertMenuItemA(menu, count, false, &item);
    }

    long choice(::TrackPopupMenu(menu,
                                 TPM_LEFTALIGN           |
                                     TPM_TOPALIGN        |
                                     TPM_LEFTBUTTON      |
                                     TPM_HORPOSANIMATION |
                                     TPM_VERPOSANIMATION |
                                     TPM_NONOTIFY        |
                                     TPM_RETURNCMD,
                                 x, y, 0, parent, 0));

    ::DestroyMenu(menu);

    if (choice == 0)
        return false;

    result = *(the_first + choice - 1);

    return true;
}

/****************************************************************************************************/

void set_font(HWND window, int uxtheme_type)
{
    LOGFONTW        log_font = { 0 };
    HFONT           font = 0;

    //
    // Use the metrics to obtain the correct font for this widget.
    //
    metrics::set_window(window);

    if (metrics::get_font(uxtheme_type, log_font))
    {
        //
        // Create a font from the LOGFONT structure.
        //
        font = ::CreateFontIndirectW(&log_font);
    }
    assert(font);

    ::SendMessage(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
}

/****************************************************************************************************/

LONG_PTR get_user_reference(HWND control)
{
    assert(control);

    return ::GetWindowLongPtr(control, GWLP_USERDATA);
}

/****************************************************************************************************/

modifiers_t convert_modifiers(ULONG os_modifiers)
{
    modifiers_t result(modifiers_none_s);

#define ADOBE_MAPMOD(osmod, adobemod)   if (os_modifiers == (osmod)) result = result | (adobemod)
    //
    // Note that VK_MENU is the ALT key.
    //
    ADOBE_MAPMOD(VK_CAPITAL,    modifiers_caps_lock_s);
    ADOBE_MAPMOD(VK_CONTROL,    modifiers_any_control_s);
    ADOBE_MAPMOD(VK_SHIFT,      modifiers_any_shift_s);
    ADOBE_MAPMOD(VK_MENU,       modifiers_any_option_s);

#undef ADOBE_MAPMOD

    return result;
}

/****************************************************************************************************/

modifiers_t convert_modifiers(BYTE keyboard_state[256])
{
    modifiers_t result(modifiers_none_s);

    if (keyboard_state[VK_CAPITAL] & 0x80)  result |= modifiers_caps_lock_s;
    if (keyboard_state[VK_LSHIFT] & 0x80)   result |= modifiers_left_shift_s;
    if (keyboard_state[VK_RSHIFT] & 0x80)   result |= modifiers_right_shift_s;
    if (keyboard_state[VK_LCONTROL] & 0x80) result |= modifiers_left_control_s;
    if (keyboard_state[VK_RCONTROL] & 0x80) result |= modifiers_right_control_s;
    if (keyboard_state[VK_LMENU] & 0x80)    result |= modifiers_left_option_s;
    if (keyboard_state[VK_RMENU] & 0x80)    result |= modifiers_right_option_s;

    return result;
}

/****************************************************************************************************/

modifiers_t modifier_state()
{
    BYTE keyboard_state[256] = { 0 };

    ::GetKeyboardState(&keyboard_state[0]);

    return convert_modifiers(keyboard_state);
}

/****************************************************************************************************/

platform_display_type get_top_level_window(platform_display_type thing)
{
    return ::GetAncestor(thing, GA_ROOT);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
