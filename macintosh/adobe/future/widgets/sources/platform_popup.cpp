/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/widgets/headers/platform_popup.hpp>

#include <iostream>
#include <map>

#include <boost/type_traits.hpp>
#include <boost/cstdint.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/enum_ops.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/istream_fwd.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/memory.hpp>
#include <adobe/unicode.hpp>
#include <adobe/xstring.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(popup_t, "<xstr id='metric:popup'/>");

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

std::string get_popup_menu_item_text(const adobe::popup_t& widget, ::UInt16 index)
{
    ::MenuRef       menu(0);
    ::CFStringRef   menu_item(0);

    adobe::implementation::get_widget_data(widget.control_m, kControlEntireControl, kControlPopupButtonMenuRefTag, menu);

    ADOBE_REQUIRE_STATUS(::CopyMenuItemTextAsCFString(menu, index, &menu_item));

    adobe::auto_cfstring_t auto_string(menu_item);

    return adobe::explicit_cast<std::string>(menu_item);
}

/**************************************************************************************************/

#if 0

::MenuRef create_menu()
{
    ::MenuRef menu(0);

    ADOBE_REQUIRE_STATUS(::CreateNewMenu(256, kMenuAttrAutoDisable, &menu)); // REVISIT FIXED VALUE

    return menu;
}

#endif

/**************************************************************************************************/

void initialize(adobe::popup_t& control, ::HIViewRef parent)
{
    assert(!control.control_m);

    static const ::Rect bounds_s = { 0, 0, 1024, 1024 };
    static const long manually_managed_menu_id(-12345);

    ADOBE_REQUIRE_STATUS(::CreatePopupButtonControl(::GetControlOwner(parent),
                                                    &bounds_s,
                                                    adobe::explicit_cast<adobe::auto_cfstring_t>(control.name_m).get(),
                                                    manually_managed_menu_id, false, -1,
                                                    popupTitleLeftJust,
                                                    ::Style(popupTitleNoStyle),
                                                    &control.control_m));

    control.key_handler_m.popup_m = &control;
    control.key_handler_m.handler_m.install(control.control_m);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);

    control.using_label_m = !control.name_m.empty();

    adobe::implementation::set_theme(control, control.theme_m);

    // These are pretty meaningless -- they're not managed by the popup based on how many
    // menu items currently exist in the widget, so we set them to fixed numbers at the onset
    // of the widget so we don't have to play with them anywhere else.

    ::SetControl32BitMinimum(control.control_m, 0);
    ::SetControl32BitMaximum(control.control_m, 4096); // REVISIT (fbrereto) : more? less?
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

template <>
inline dictionary_t extra_widget_context(const popup_t& w)
{
    static_name_t has_label(w.using_label_m ? "true" : "false");
    dictionary_t  result;

    result.insert(std::make_pair(static_name_t("label"), any_regular_t(has_label)));

    return result;
}

/**************************************************************************************************/

template <>
std::string get_field_text<popup_t>(const popup_t& widget)
{
    ::UInt16 value(static_cast< ::UInt16 >(implementation::get_value(widget)));

    return get_popup_menu_item_text(widget, value);
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

::OSStatus popup_value_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                               ::EventRef            /*event*/)
{
    assert(!setter_m.empty() || !extended_setter_m.empty());

    if (widget_m.menu_items_m.empty() ||
        widget_m.type_2_debounce_m)
        return noErr;

    std::size_t popup_value(static_cast<std::size_t>(implementation::get_value(widget_m.control_m) - 1));

    if (widget_m.custom_m)
        popup_value -= 2; // skip past the custom additions

    if (popup_value < widget_m.menu_items_m.size())
    {
        if (setter_m)
            setter_m(widget_m.menu_items_m[popup_value].second);

        if (extended_setter_m)
            extended_setter_m(widget_m.menu_items_m[popup_value].second,
                              implementation::convert_modifiers(::GetCurrentEventKeyModifiers()));
    }

    return noErr;
}

/**************************************************************************************************/

::OSStatus popup_key_handler_t::handle_event(::EventHandlerCallRef next,
                                             ::EventRef            event)
{
    assert(popup_m);

    OSStatus result(noErr);

    if (::GetEventClass(event) != kEventClassTextInput)
        return eventNotHandledErr;

    if (::GetEventKind(event) != kEventTextInputUnicodeForKeyEvent)
        return eventNotHandledErr;

    std::vector< ::UniChar >    buffer;
    ::UInt32                    actualSize; 
    ::ControlRef                control(popup_m->control_m);
    ::MenuRef                   menu(::GetControlPopupMenuHandle(control));
    std::size_t                 menu_item_count(::CountMenuItems(menu));

    ADOBE_REQUIRE_STATUS(::GetEventParameter(   event, kEventParamTextInputSendText,
                                                typeUnicodeText, NULL, 0, &actualSize, NULL));

    buffer.reserve(actualSize + 1);

    ADOBE_REQUIRE_STATUS(::GetEventParameter(   event, kEventParamTextInputSendText,
                                                typeUnicodeText, NULL, actualSize, NULL, &buffer[0]));

    std::string text(implementation::convert_utf(&buffer[0], actualSize).c_str());

    if (text[0] >= 0x1C && text[0] <= 0x1F)
    {
        // arrow keys. navigate popup accordingly.
        std::size_t cur_value = static_cast<std::size_t>(implementation::get_value(control));

        if (text[0] == kLeftArrowCharCode || text[0] == kUpArrowCharCode)
        {
            if (cur_value == 1)
                system_beep();
            else
                ::SetControl32BitValue(control, static_cast<long>(cur_value - 1));
        }
        else if (text[0] == kRightArrowCharCode || text[0] == kDownArrowCharCode)
        {
            if (cur_value == menu_item_count)
                system_beep();
            else
                ::SetControl32BitValue(control, static_cast<long>(cur_value + 1));
        }
    }
    else if (text[0] == kTabCharCode || text[0] == kSpaceCharCode)
    {
        result = ::CallNextEventHandler(next, event);
    }
    else if (menu_item_count != 0)
    {
        implementation::set_popup_with_text(control, text, ::EventTimeToTicks(::GetEventTime(event)));
    }

    return result;
}

/**************************************************************************************************/

popup_t::popup_t(const std::string& name,
                 const std::string& alt_text,
                 const std::string& custom_item_name,
                 const menu_item_t* first,
                 const menu_item_t* last,
                 theme_t            theme) :
    control_m(0),
    theme_m(theme),
    name_m(name),
    alt_text_m(alt_text),
    label_width_m(0),
    menu_items_m(first, last),
    value_handler_m(*this),
    type_2_debounce_m(false),
    custom_m(false),
    custom_item_name_m(custom_item_name)
{ }

/**************************************************************************************************/

void popup_t::reset_menu_item_set(const menu_item_t* first, const menu_item_t* last)
{
    assert(control_m);

    custom_m = false;

    menu_item_set_t new_set(first, last);

    std::swap(new_set, menu_items_m);

    ::MenuRef   menu(0);

    ADOBE_REQUIRE_STATUS(::CreateNewMenu(256, kMenuAttrAutoDisable, &menu)); // REVISIT FIXED VALUE

    while (first != last)
    {
        bool is_sep(first->first == std::string("__separator") || first->first == std::string("-"));

        ADOBE_REQUIRE_STATUS(::AppendMenuItemTextWithCFString(
            menu,
            is_sep ? CFSTR("-") : explicit_cast<auto_cfstring_t>(first->first).get(),
            is_sep ? kMenuItemAttrSeparator : kMenuItemAttrIgnoreMeta,
            0, 0));

        ++first;
    }

    ::SetControlPopupMenuHandle(control_m, menu);

    enable(!menu_items_m.empty());

    if (menu_items_m.empty())
        return;

    display(last_m);
}

/**************************************************************************************************/

void popup_t::measure(extents_t& result)
{
    assert(control_m);

    result = implementation::measure(*this);

    if (!using_label_m)
        return;

    extents_t attrs(implementation::measure_theme_text(name_m, theme_m));

    label_width_m = attrs.width();

    result.width() = std::max<std::size_t>(result.width(), label_width_m);

    result.width() = std::min<std::size_t>(result.width(), 300); // REVISIT (fbrereto) : fixed width

    result.horizontal().guide_set_m.push_back(label_width_m);
}

/**************************************************************************************************/

void popup_t::place(const place_data_t& place_data)
{
    place_data_t my_data(place_data);

    if (!place_data.horizontal().guide_set_m.empty() && place_data.horizontal().guide_set_m[0] != label_width_m)
    {
        long difference = place_data.horizontal().guide_set_m[0] - label_width_m;

        width(my_data) -= difference;
        left(my_data)  += difference;
    }

    implementation::set_bounds(*this, my_data);
}

/**************************************************************************************************/

void popup_t::enable(bool make_enabled)
{
    assert(control_m);

    implementation::set_active(control_m, make_enabled);
}

/**************************************************************************************************/

void popup_t::display(const any_regular_t& value)
{
    assert(control_m);

    last_m = value;

    popup_t::menu_item_set_t::iterator first(menu_items_m.begin());
    popup_t::menu_item_set_t::iterator last(menu_items_m.end());

    for (; first != last; ++first)
    {
        if ((*first).second == value)
        {
            if (custom_m)
            {
                custom_m = false;

                ::MenuRef menu(::GetControlPopupMenuHandle(control_m));

                ::DeleteMenuItem(menu, 1); // delete 'custom'
                ::DeleteMenuItem(menu, 1); // delete separator

                ::Draw1Control(control_m);
            }

            type_2_debounce_m = true;

            ::SetControl32BitValue(control_m, (first - menu_items_m.begin()) + 1);

            type_2_debounce_m = false;

            return;
        }
    }

    display_custom();
}

/**************************************************************************************************/

void popup_t::display_custom()
{
    if (custom_m)
        return;

    custom_m = true;

    ::MenuRef menu(::GetControlPopupMenuHandle(control_m));

    if (menu == 0)
        return;

    ADOBE_REQUIRE_STATUS(::InsertMenuItemTextWithCFString(
        menu, CFSTR(""), 0, kMenuItemAttrSeparator, 0));

    ADOBE_REQUIRE_STATUS(::InsertMenuItemTextWithCFString(
        menu, explicit_cast<auto_cfstring_t>(custom_item_name_m).get(),
        0, kMenuItemAttrIgnoreMeta, 0));

    type_2_debounce_m = true;

    ::SetControl32BitValue(control_m, 1);

    type_2_debounce_m = false;

    ::Draw1Control(control_m);
}

/**************************************************************************************************/

void popup_t::select_with_text(const std::string& text)
{
    // REVISIT (fbrereto) : make this into an assert, but check the case when there is
    //                      no unit popup associated with an edit_number field.
    if (control_m == 0)
        return;

    implementation::set_popup_with_text(control_m, text, implementation::get_current_tick_time());
}

/**************************************************************************************************/

void popup_t::monitor(const setter_type& proc)
{
    assert(control_m);

    value_handler_m.setter_m = proc;

    if (value_handler_m.handler_m.is_installed() == false)
        value_handler_m.handler_m.install(control_m);
}

/**************************************************************************************************/

void popup_t::monitor_extended(const extended_setter_type& proc)
{
    assert(control_m);

    value_handler_m.extended_setter_m = proc;

    if (value_handler_m.handler_m.is_installed() == false)
        value_handler_m.handler_m.install(control_m);
}

/**************************************************************************************************/

template <>
platform_display_type insert(display_t&             display,
                             platform_display_type& parent,
                             popup_t&        element)
{
    initialize(element, parent);

    element.reset_menu_item_set(element.menu_items_m);

    return display.insert(parent, element.control_m);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
