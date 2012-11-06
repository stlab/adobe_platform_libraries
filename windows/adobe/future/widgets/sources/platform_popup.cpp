/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1

#include <windows.h>
#include <Commctrl.h>
#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h> //Yes, we include this twice -- read the top of the file

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/platform_popup.hpp>
#include <adobe/placeable_concept.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

enum metrics {
    gap = 4 // Measured as space from popup to label.
};

/****************************************************************************************************/

void clear_menu_items(adobe::popup_t& control)
{
    assert(control.control_m);

    control.menu_items_m.erase(control.menu_items_m.begin(), control.menu_items_m.end());

    if (::SendMessage(control.control_m, CB_RESETCONTENT, 0, 0) == CB_ERR)
        ADOBE_THROW_LAST_ERROR;
}

/****************************************************************************************************/

LRESULT CALLBACK popup_subclass_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR ptr, DWORD_PTR /* ref */)
{
    adobe::popup_t& control(*reinterpret_cast<adobe::popup_t*>(ptr));

    assert(control.control_m);

    //
    // Inform everybody that the selection is changing.
    //
    if (message == WM_COMMAND &&
        HIWORD(wParam) == CBN_SELCHANGE &&
        control.type_2_debounce_m == false &&
        (control.value_proc_m.empty() == false ||
         control.extended_value_proc_m.empty() == false))
    {
        long new_index(static_cast<long>(::SendMessage(control.control_m, CB_GETCURSEL, 0, 0)));

        if (control.custom_m)
            --new_index;

        if (control.value_proc_m)
            control.value_proc_m(control.menu_items_m.at(new_index).second);

        if (control.extended_value_proc_m)
            control.extended_value_proc_m(control.menu_items_m.at(new_index).second, adobe::modifier_state());
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

/****************************************************************************************************/

void initialize(adobe::popup_t& control, HWND parent)
{
    assert(!control.control_m);

    control.control_m = ::CreateWindowEx(WS_EX_COMPOSITED, WC_COMBOBOX,
                                         NULL,
                                         WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL,
                                         0, 0, 0, 0,
                                         parent,
                                         0,
                                         ::GetModuleHandle(NULL),
                                         NULL);

    if (control.control_m == NULL)
        ADOBE_THROW_LAST_ERROR;

    ::SetWindowSubclass(control.control_m, popup_subclass_proc, reinterpret_cast<UINT_PTR>(&control), 0);

    adobe::set_font(control.control_m, CP_DROPDOWNBUTTON);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);
}

/****************************************************************************************************/

bool has(const adobe::popup_t& control, const adobe::popup_t::model_type& value)
{
    adobe::popup_t::menu_item_set_t::const_iterator iter(control.menu_items_m.begin());
    adobe::popup_t::menu_item_set_t::const_iterator last(control.menu_items_m.end());

    for (; iter != last; ++iter)
        if (iter->second == value)
            return true;

    return false;
}

/****************************************************************************************************/

void set_menu_item_set(adobe::popup_t& p, const adobe::popup_t::menu_item_t* first, const adobe::popup_t::menu_item_t* last)
{
    p.custom_m = false;

    for (; first != last; ++first)
    {
        // MM: Revisit. Is there a way to have disabled separators in combo boxes?
        // Since I don't know a way I intercept -'s here. (Dashes inidcate separators
        // on the macintosh and also in eve at the moment). Alternative implementation
        // ideas:
        // Combobox with owner draw and manual separator support. 
        // Windows forms/NET 2.0 Toolstrip. Or maybe better to do without separators.

        if(first->first != "-" && first->first != "__separator") 
            p.menu_items_m.push_back(*first);
    }
}

/****************************************************************************************************/

void message_menu_item_set(adobe::popup_t& p)
{
    assert(p.control_m);

    for(adobe::popup_t::menu_item_set_t::const_iterator first=p.menu_items_m.begin(), last=p.menu_items_m.end(); first != last; ++first)
    {
        if (::SendMessageW(p.control_m, CB_ADDSTRING, 0, (LPARAM) hackery::convert_utf(first->first).c_str()) == CB_ERR)
            ADOBE_THROW_LAST_ERROR;
    }
    ::SendMessage(p.control_m, CB_SETCURSEL, 0, 0);
    
    p.enable(!p.menu_items_m.empty());

    if (p.menu_items_m.empty())
        return;

    p.display(p.last_m);

}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

popup_t::popup_t(const std::string& name,
                 const std::string& alt_text,
                 const std::string& custom_item_name,
                 const menu_item_t* first,
                 const menu_item_t* last,
                 theme_t            theme) :
    control_m(0),
    theme_m(theme),
    name_m(name, alt_text, 0, theme),
    alt_text_m(alt_text),
    static_baseline_m(0),
    static_height_m(0),
    popup_baseline_m(0),
    popup_height_m(0),
    using_label_m(!name.empty()),
    custom_m(false),
    custom_item_name_m(custom_item_name)
{
    ::set_menu_item_set(*this, first, last);
}

/****************************************************************************************************/

void popup_t::measure(extents_t& result)
{
    assert(control_m);
    //
    // Make sure that metrics_t is initialized.
    //
    metrics::set_window(control_m);
    //
    // The popup_t has multiple text items. We need to find the one with
    // the widest extents (when drawn). Then we can make sure that we get
    // enough space to draw our largest text item.
    //
    menu_item_set_t::iterator first(menu_items_m.begin());
    menu_item_set_t::iterator last(menu_items_m.end());
    RECT largest_extents = { 0, 0, 0, 0 };
    bool have_extents = false;
    //
    // Now iterate through all of our text.
    //
    while (first != last)
    {
        //
        // Discover the extents of this text!
        //
        RECT extents;
        if (metrics::get_text_extents(CP_DROPDOWNBUTTON, hackery::convert_utf(first->first), extents))
        {
            //
            // Alright, we were able to obtain the required extents.
            // Now we just need to see if they are larger than the
            // ones we already have.
            //
            if ((extents.right - extents.left) > (largest_extents.right - largest_extents.left))
                largest_extents = extents;
            have_extents = true;
        }
        ++first;
    }
    //
    // We don't really use much of UxTheme to discover the bounds of
    // the combobox. We can use the GetComboboxInfo function to get
    // most of the information we require (such as where the text will
    // lie).
    //
    TEXTMETRIC font_metrics;
    int border;
    bool have_metrics = metrics::get_font_metrics(CP_DROPDOWNBUTTON, font_metrics);
    bool have_border = metrics::get_integer(CP_DROPDOWNBUTTON, TMT_BORDERSIZE, border);
    COMBOBOXINFO cbi;
    cbi.cbSize = sizeof(cbi);
    if (GetComboBoxInfo(control_m, &cbi))
    {
        RECT text = { 0, 0, 0, 0 };
        // currently unused
        //  RECT size = { 0, 0, 0, 0 };
        WINDOWINFO wi = { 0 };
        wi.cbSize = sizeof(wi);
        if (!GetWindowInfo(control_m, &wi)) ADOBE_THROW_LAST_ERROR;
        //
        // Figure out the borders around the text entry area.
        //
        text.left = wi.rcClient.left - wi.rcWindow.left + cbi.rcItem.left;
        text.right = wi.rcWindow.right - wi.rcClient.right + cbi.rcItem.right;
        text.top = wi.rcClient.top - wi.rcWindow.top + cbi.rcItem.top;
        text.bottom = wi.rcWindow.bottom - wi.rcWindow.bottom + cbi.rcItem.bottom;
        //
        // Figure out the dimensions for the entire control.
        //
        result.width() = text.left + largest_extents.right - largest_extents.left + cbi.rcButton.right - cbi.rcButton.left;
        result.height() = wi.rcWindow.bottom - wi.rcWindow.top;
        //
        // Deduce the baseline from the text rectangle.
        //
        int baseline = 0;
        if (have_metrics) {
            baseline = text.top + font_metrics.tmAscent;
            if (have_border) baseline += border;
        }
        result.vertical().guide_set_m.push_back(baseline);
    } else ADOBE_THROW_LAST_ERROR;

    //
    // If we have a label (always on our left side?) then we
    // need to add the size of the label to our result. We try
    // to align the label with the popup by baseline. Which is
    // kind of what Eve does, so it's bad that we do this
    // ourselves, really...
    //
    if (!using_label_m)
        return;
    //
    // We store the height of the label, from this we can
    // figure out how much to offset it when positioning
    // the widgets in set_bounds.
    //
    extents_t label_bounds;
    measure_label_text(name_m, label_bounds, ::GetParent(control_m));
    static_height_m = label_bounds.height();
    static_baseline_m = label_bounds.vertical().guide_set_m[0];
    //
    // Now we can align the label within the vertical
    // slice of the result. This doesn't do anything if
    // the label is shorter than the popup.
    //
    align_slices(result.vertical(), label_bounds.vertical());
    //
    // Add the width of the label (plus a gap) to the
    // resulting width.
    //
    result.width() += gap + label_bounds.width();

    //
    // Don't let the width of the popup go too crazy now...
    //
    result.width() = std::min<long>(static_cast<long>(result.width()), 300); // REVISIT (fbrereto) : fixed width

    //
    // Add a point-of-interest where the label ends.
    // We put the label to the left of the popup.
    //
    result.horizontal().guide_set_m.push_back(label_bounds.width());

    return;
}

/****************************************************************************************************/

void popup_t::place(const place_data_t& place_data)
{
    using adobe::place;

    assert(control_m);

    place_data_t local_place_data(place_data);

    //
    // If we have a label then we need to allocate space for it
    // out of the space we have been given.
    //
    if (using_label_m)
    {
        //
        // The vertical offset of the label is the geometry's
        // baseline - the label's baseline.
        //
        assert(place_data.vertical().guide_set_m.empty() == false);
        long baseline = place_data.vertical().guide_set_m[0];

        //
        // Apply the vertical offset.
        //
        place_data_t label_place_data;
        label_place_data.horizontal().position_m = left(place_data);
        label_place_data.vertical().position_m = top(place_data) + (baseline - static_baseline_m);

        //
        // The width of the label is the first horizontal
        // point of interest.
        //
        assert(place_data.horizontal().guide_set_m.empty() == false);
        width(label_place_data) = place_data.horizontal().guide_set_m[0];
        height(label_place_data) = static_height_m;

        //
        // Set the label dimensions.
        //
        place(name_m, label_place_data);

        //
        // Now we need to adjust the position of the popup
        // widget.
        //
        long width = gap + adobe::width(label_place_data);
        local_place_data.horizontal().position_m += width;
        adobe::width(local_place_data) -= width;
    }
    //
    // On Win32, you give a combo box the height it should have
    // when it's fully extended (which seems a bit like a hackish
    // implementation detail poking through). Here we ensure that
    // the combo box is maximally the size of 10 elements (so that
    // we don't go over the edges of the screen if we have a huge
    // number of elements).
    //
    // REVISIT (ralpht) : fixed value.
    //
    height(local_place_data) = height(place_data) + std::min<long>(static_cast<long>((menu_items_m.size() + 1)), 10) * height(place_data);

    implementation::set_control_bounds(control_m, local_place_data);
}

/****************************************************************************************************/

void popup_t::enable(bool make_enabled)
{
    assert(control_m);

    ::EnableWindow(control_m, make_enabled);
}



/****************************************************************************************************/

void popup_t::reset_menu_item_set(const menu_item_t* first, const menu_item_t* last)
{
    assert(control_m);

    clear_menu_items(*this);

    ::set_menu_item_set(*this, first, last);

    ::message_menu_item_set(*this);

}

/****************************************************************************************************/

void popup_t::display(const model_type& value)
{
    assert(control_m);

    last_m = value;

    menu_item_set_t::iterator  first(menu_items_m.begin());
    menu_item_set_t::iterator  last(menu_items_m.end());

    /* REVISIT (sparent) : Should be some way to use lookup for this. */

    for (; first != last; ++first)
    {
        if ((*first).second == value)
        {
            if (custom_m)
            {
                custom_m = false;

                ::SendMessage(control_m, CB_DELETESTRING, 0, 0);
            }

            std::ptrdiff_t index(first - menu_items_m.begin());

            type_2_debounce_m = true;

            ::SendMessage(control_m, CB_SETCURSEL, index, 0);

            type_2_debounce_m = false;

            return;
        }
    }

    display_custom();
}

/****************************************************************************************************/

void popup_t::display_custom()
{
    if (custom_m)
        return;

    custom_m = true;

    ::SendMessageA(control_m, CB_INSERTSTRING, 0, (LPARAM)custom_item_name_m.c_str());

    type_2_debounce_m = true;

    ::SendMessage(control_m, CB_SETCURSEL, 0, 0);

    type_2_debounce_m = false;
}

/****************************************************************************************************/

void popup_t::select_with_text(const std::string& text)
{
    assert(control_m);

    long old_index(static_cast<long>(::SendMessage(control_m, CB_GETCURSEL, 0, 0)));

    ::SendMessageW(control_m, CB_SELECTSTRING, (WPARAM) -1, (LPARAM) hackery::convert_utf(text.c_str()).c_str());

    if (value_proc_m.empty())
        return;

    long new_index(static_cast<long>(::SendMessage(control_m, CB_GETCURSEL, 0, 0)));

    if (new_index != old_index)
        value_proc_m(menu_items_m.at(new_index).second);
}

/****************************************************************************************************/

void popup_t::monitor(const setter_type& proc)
{
    assert(control_m);

    value_proc_m = proc;
}

/****************************************************************************************************/

void popup_t::monitor_extended(const extended_setter_type& proc)
{
    assert(control_m);

    extended_value_proc_m = proc;
}

/****************************************************************************************************/

// REVISIT: MM--we need to replace the display_t mechanism with concepts/any_*/container idiom for event and drawing system.

template <>
platform_display_type insert<popup_t>(display_t& display, platform_display_type& parent, popup_t& element)
{
    HWND parent_hwnd(parent);

    if (element.using_label_m)
        insert(display, parent, element.name_m);

    ::initialize(element, parent_hwnd);
    ::message_menu_item_set(element);

    platform_display_type result(display.insert(parent, element.control_m));

    return result;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
