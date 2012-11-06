/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/future/widgets/headers/platform_edit_text.hpp>

#include <windows.h>
#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file
#include <tchar.h>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/placeable_concept.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/controller_concept.hpp>

const int gap=4;

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

void fix_newlines(std::string& string)
{
    // check for cases where \n is not preceeded by \r

    std::string::size_type result(0);

    while (true)
    {
        result = string.find("\n", result);

        if (result == std::string::npos)
            break;

        if (result == 0 || string[result - 1] != '\r')
            string.insert(result, "\r");

        ++result;
    }
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

LRESULT CALLBACK edit_text_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR ptr, DWORD_PTR /* ref */);

/*************************************************************************************************/

void edit_text_t::display(const model_type& value) // values that come in from Adam
{
    std::string string(value);

    if (string != value_m)
    {
        fix_newlines(string);

        value_m = string;

        set_field_text(string);
    }
}

/*************************************************************************************************/

void edit_text_t::edit_text_label_hit(modifiers_t)
{
    ::SetFocus(control_m);
}


/****************************************************************************************************/

void edit_text_t::initialize(HWND parent)
{
	if (using_label_m)
        signal_label_hit(boost::bind(&edit_text_t::edit_text_label_hit, this, _1));

    control_m = ::CreateWindowEx( WS_EX_COMPOSITED | WS_EX_CLIENTEDGE,
                    _T("EDIT"),
                    NULL,
                    get_window_style(),
                    0, 0, 100, 20,
                    parent,
                    0,
                    ::GetModuleHandle(NULL),
                    NULL);

    if (!control_m)
        ADOBE_THROW_LAST_ERROR;

    set_theme(theme_m);

    ::SetWindowSubclass(control_m, edit_text_window_proc, reinterpret_cast<UINT_PTR>(this), 0);

    if (!alt_text_m.empty())
        implementation::set_control_alt_text(control_m, alt_text_m);
}

/****************************************************************************************************/

edit_text_t::edit_text_t(const edit_text_ctor_block_t& block) : 
    name_m(block.name_m, block.alt_text_m, 0, block.theme_m),
    alt_text_m(block.alt_text_m),
    field_text_m(),
    using_label_m(!block.name_m.empty()),
    rows_m(block.num_lines_m),
    cols_m(block.min_characters_m),
    max_cols_m(block.max_characters_m),
    scrollable_m(block.scrollable_m),
    password_m(block.password_m),
    edit_baseline_m(0),
    edit_height_m(0),
    static_baseline_m(0),
    static_height_m(0),
    type_2_debounce_m(false)
{ 
}

/****************************************************************************************************/
extents_t calculate_edit_bounds(HWND control, int cols, int rows);


void edit_text_t::measure(extents_t& result)
{
    assert(control_m);
    //
    // The calculate_edit_bounds function can figure out the size this edit box
    // should be, based on the number of rows and columns.
    //
    result = calculate_edit_bounds(control_m, cols_m, rows_m);
    //
    // Store the height and baseline so that we can correctly align the edit widget
    // in set_bounds.
    //
    edit_height_m = result.height();
    if (!result.vertical().guide_set_m.empty())
        edit_baseline_m = result.vertical().guide_set_m[0];
    //
    // If we have a label then we need to make extra space
    // for it.
    //
    if (!using_label_m) return;
    extents_t label_bounds;
    measure_label_text(get_label(), label_bounds, ::GetParent(control_m));
    //
    // Make sure that the height can accomodate both the label
    // and the edit widget.
    //
    align_slices(result.vertical(), label_bounds.vertical());
    //
    // We put the label on the left side of the edit box, and
    // place a point of interest at the end of the label, so
    // that colon alignment can be performed.
    //

    result.width() += gap + label_bounds.width();
    result.horizontal().guide_set_m.push_back(label_bounds.width());
    //
    // We use the height and baseline of the label to size and
    // align it in set_bounds.
    //
    static_height_m = label_bounds.height();
    static_baseline_m = label_bounds.vertical().guide_set_m[0];
}

/****************************************************************************************************/

void edit_text_t::place(const place_data_t& place_data)
{
    using adobe::place;

    assert(control_m);

    place_data_t local_place_data(place_data);

    long baseline = local_place_data.vertical().guide_set_m[0];

    if (using_label_m)
    {
        //
        // We're using a label. We need to extract the label width from the
        // points of interest in the given geometry, and make sure that we
        // correctly translate the label for baseline alignment.
        //
        place_data_t    label_place_data;
        label_place_data.horizontal().position_m = left(local_place_data);
        label_place_data.vertical().position_m = top(local_place_data);

        //
        // We stored the height of the label in best_bounds. The width of
        // the label can be discovered via the first horizontal point of
        // interest.
        //
        height(label_place_data) = static_height_m;
        width(label_place_data) = local_place_data.horizontal().guide_set_m[0];
        //
        // Translate the label vertically for baseline alignment with the
        // edit widget. We stored the label's baseline in best_bounds.
        //
        label_place_data.vertical().position_m += baseline - static_baseline_m;
        place(get_label(), label_place_data);

        local_place_data.horizontal().position_m += width(label_place_data) + gap;
        width(local_place_data) -= width(label_place_data) + gap;
    }
    //
    // We might need to translate the edit widget vertically, for baseline
    // alignment.
    //
    local_place_data.vertical().position_m += baseline - edit_baseline_m;

    // REVISIT (thw) : Do we need to adapt the height for baseline alignment?

    implementation::set_control_bounds(control_m, local_place_data);
}

/****************************************************************************************************/

label_t& edit_text_t::get_label()
{
    return name_m;
}

/****************************************************************************************************/

void edit_text_t::enable(bool active)
{
    using adobe::enable;

    assert(control_m);

    ::EnableWindow(control_m, active);

    if (using_label_m)
        enable(get_label(), active);
}

/****************************************************************************************************/

void edit_text_t::set_theme(theme_t theme)
{
    theme_m = theme;

    set_font(control_m, EP_EDITTEXT);

//    if (using_label_m)
//        get_label() = label_t(get_control_string(get_label()), alt_text_m, 0, theme_m);
}


/****************************************************************************************************/

void edit_text_t::set_field_text(const std::string& text)
{
    if (type_2_debounce_m)
        return;

    assert(control_m);

    // REVISIT (fbrereto) : Currently we require the line endings to be whatever the platform wants.
    //                      I'm not sure we want to make it the burden of the widget to handle all
    //                      forms of line endings.

    type_2_debounce_m = true;

    ::SendMessageW(control_m, WM_SETTEXT, 0, (LPARAM) hackery::convert_utf(text.c_str()).c_str());

    type_2_debounce_m = false;
}


/****************************************************************************************************/

void edit_text_t::set_selection(long , long )
{
    assert(control_m);
}

/****************************************************************************************************/

void edit_text_t::signal_pre_edit(edit_text_pre_edit_proc_t proc)
{
    assert(control_m);

    if (!pre_edit_proc_m)
        pre_edit_proc_m = proc;
}

/****************************************************************************************************/

void edit_text_t::monitor(setter_type proc)
{
    if (!post_edit_proc_m)
        post_edit_proc_m = proc;
}

/****************************************************************************************************/

void edit_text_t::signal_label_hit(edit_text_label_hit_proc_t proc)
{
//    if (using_label_m)
//        get_label().signal_hit(proc);
}

/****************************************************************************************************/

long edit_text_t::get_window_style() const
{
    //
    // Make it so that the edit box automatically scrolls when the
    // user types beyond the visible region or clicks and drags
    // inside the text box.
    //
    // Also set a couple other flags we always want set.
    //
    long edit_style = ES_AUTOHSCROLL | ES_AUTOVSCROLL |
                      WS_CHILD | WS_TABSTOP | WS_VISIBLE;

    //
    // We can set the ES_MULTILINE style if we have multiple rows.
    // The ES_WANTRETURN flag tells the edit control to interpret
    // the return key as a newline/return, rather than to tell the
    // parent window about it.
    //
    if (rows_m > 1) 
        edit_style |= ES_MULTILINE | ES_WANTRETURN;

    //
    // The WS_VSCROLL and WS_HSCROLL styles apply to any window.
    //
    if (scrollable_m)
        edit_style |= WS_VSCROLL | WS_HSCROLL;

    //
    // The ES_PASSWORD style applies to an edit which needs to
    // hide the characters entered.
    //
    if (password_m)
        edit_style |= ES_PASSWORD;

    return edit_style;
}

/****************************************************************************************************/
//
/// This callback handles incoming keypresses and uses the pre_edit signal to determine
/// if the key press should be allowed into the editbox or not.
///
/// The rationale for having a custom window procedure for edit controls is that there
/// is no way to handle single key-press events before they get processed by the edit
/// control and merged with the existing text (meaning we would have to recalculate the
/// change which occured).
//
LRESULT CALLBACK edit_text_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR ptr, DWORD_PTR ref)
{
    edit_text_t& edit_text(*reinterpret_cast<edit_text_t*>(ptr));
    bool         squelch(false);
    bool         keyevent(message == WM_KEYDOWN || message == WM_CHAR);
    wchar_t      character(keyevent ? static_cast<wchar_t>(wParam) : 0);

    // Looking for the arrow key code? It's been moved to edit_number::handle_key

    if (message == WM_MOUSEWHEEL && edit_text.pre_edit_proc_m)
    {
        if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            character = VK_UP;
        else if(GET_WHEEL_DELTA_WPARAM(wParam) < 0)
            character = VK_DOWN;

        if (character)
        {
            char c(static_cast<char>(character));

            switch (character)
            {
                case VK_UP:   c = 30; break;
                case VK_DOWN: c = 31; break;
            }

            std::string str(&c, 1);

            edit_text.pre_edit_proc_m(str, squelch);
        }

        return 0;
    }
    else if (message == WM_CHAR)
    {
        std::string old_value(get_control_string(edit_text));
        DWORD       old_selection_start(0);
        DWORD       old_selection_end(0);

        ::SendMessage(edit_text.control_m,
                      EM_GETSEL,
                      (WPARAM) &old_selection_start,
                      (LPARAM) &old_selection_end);

        // pass the event to the control to get the new string
        LRESULT result = ::DefSubclassProc(window, message, wParam, lParam);

        std::string new_value(get_control_string(edit_text));

        // if the text string is the same, we're done here.
        if (new_value == old_value)
            return result;

        squelch = edit_text.rows_m == 1 &&
                  edit_text.max_cols_m > 0 &&
                  static_cast<std::size_t>(edit_text.max_cols_m) < new_value.size();

        // send the new text off to the prefilter, but only
        // if we haven't hit our max length quota (but only
        // in the case when we're one row high)

        if (squelch == false && edit_text.pre_edit_proc_m)
            edit_text.pre_edit_proc_m(new_value, squelch);

        if (squelch)
        {
            ::MessageBeep(-1);

            edit_text.set_field_text(old_value);

            // restore the selection location, too
            ::SendMessage(edit_text.control_m, EM_SETSEL, old_selection_start, old_selection_end);
        }
        else if (edit_text.post_edit_proc_m)
        {
            edit_text.value_m = new_value;

            edit_text.post_edit_proc_m(new_value);
        }

        return result;
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/
//
/// This function will calculate the best bounds for an edit control. This is
/// a bit more complicated than it could be, because we can't get all of the
/// values we want from the UXTheme API. Fortunately we can get them from a
/// combination of User32 and UXTheme.
///
/// This function is used by the edit widget, as well as the popup widget
/// (which contains an edit widget).
///
/// \param  control the HWND of the edit control to obtain the best bounds for.
/// \param  cols    the number of columns (or characters across) the edit control should contain
/// \param  rows    the number of rows of text to contain.
///
/// \return the best bounds for the edit control, including baseline.
//
extents_t calculate_edit_bounds(HWND control, int cols, int rows)
{
    extents_t result;
    //
    // First make sure that we have a valid control.
    //
    assert(control);
    //
    // We figure out the size by sending an EM_GETRECT message to the edit
    // control. This gives us the offset which it draws at inside it's
    // client rectangle.
    //
    RECT em_rect = {0, 0, 0, 0};
    SendMessage(control, EM_GETRECT, 0, (LPARAM)&em_rect);
    //
    // Next we need to find out where the client rectangle lies within the
    // bounds of the regular window.
    // 
    WINDOWINFO wi = { 0 };
    wi.cbSize = sizeof(wi);
    if (!GetWindowInfo(control, &wi)) return result;
    //
    // Finally we need to extract the font metrics from UxTheme, so that we
    // can correctly calculate the baseline.
    //
    TEXTMETRIC font_metrics;
    RECT extents = { 0 };
    int border = 0;
    if (!metrics::set_window(control)) return result;
    if (!metrics::get_text_extents(EP_EDITTEXT, std::wstring(L"0", cols), extents)) return result;
    if (!metrics::get_font_metrics(EP_EDITTEXT, font_metrics)) return result;
    if (!metrics::get_integer(EP_EDITTEXT, TMT_BORDERSIZE, border)) return result;
    //
    // Now we can calculate the rectangle:
    //
    // Height: Top border + Bottom border + (rows * text height) + vertical text offset
    // Width : Left border + Right border + text extent
    // Baseline: Top border + font ascent + one if this has multiple lines...
    //
    RECT offsets;

    offsets.top = wi.rcClient.top - wi.rcWindow.top;
    offsets.left = wi.rcClient.left - wi.rcWindow.left;
    offsets.bottom = wi.rcWindow.bottom - wi.rcClient.bottom;
    offsets.right = wi.rcWindow.right - wi.rcClient.right;

    result.height() = offsets.top + offsets.bottom + (rows * font_metrics.tmHeight) + em_rect.top + border;
    result.width() = (extents.right - extents.left) + offsets.left + offsets.right;
    result.vertical().guide_set_m.push_back(offsets.top + font_metrics.tmAscent + border);

    return result;
}

/****************************************************************************************************/

template <>
platform_display_type insert<edit_text_t>(display_t&             display,
                                                 platform_display_type& parent,
                                                 edit_text_t&    element)
{
    HWND parent_hwnd(parent);

    if (element.using_label_m)
    {
        insert(display, parent, element.get_label());
    }

   element.initialize(parent_hwnd);

   return display.insert(parent, get_display(element));
}

/****************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
