/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

void initialize(adobe::label_t& control, ::HIViewRef parent)
{
    const static ::Rect bounds_s = { 0, 0, 1024, 1024 };

    ::ControlRef            tmp_control;
    ::HIViewTrackingAreaRef dummyref(0);
    ::WindowRef             owner(parent ? ::GetControlOwner(parent) : NULL);

    ADOBE_REQUIRE_STATUS(::CreateStaticTextControl(owner, &bounds_s,
                                                   adobe::explicit_cast<adobe::auto_cfstring_t>(control.name_m).get(),
                                                   NULL, &tmp_control));

    if (control.alt_text_m.empty() == false)
        adobe::implementation::set_control_alt_text(tmp_control, control.alt_text_m);

    ::HIViewNewTrackingArea(tmp_control, NULL, 0, &dummyref);

    control.subevent_m.control_m = tmp_control;
    control.control_m.reset(tmp_control);

    adobe::implementation::set_theme(control, control.theme_m);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

::OSStatus label_subevent_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                  ::EventRef            event)
{
    if (!callback_m)
        return noErr;

    assert (::GetEventClass(event) == kEventClassControl);

    label_subevent_t which(subevent_none_s);

    switch (::GetEventKind(event))
    {
        case kEventControlClick:
        case kEventControlHit:
            which = subevent_mouse_down_s;
            break;
        case kEventControlTrackingAreaEntered:
            which = subevent_mouse_in_s;
            break;
        case kEventControlTrackingAreaExited:
            which = subevent_mouse_out_s;
            break;
        default:
            throw std::runtime_error("unknown case");
    }

    callback_m(which);

    return noErr;
}


/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

template <>
std::string get_name<label_t>(const label_t& widget)
{
    if (!widget.control_m.get()) return std::string();

    ::CFStringRef cfstring;

    get_widget_data(widget.control_m.get(), kControlEntireControl, kControlStaticTextCFStringTag, cfstring);

    auto_cfstring_t auto_cfstring(cfstring);

    return explicit_cast<std::string>(cfstring);
}

/****************************************************************************************************/

template <>
void set_name< label_t >(label_t& control, const std::string& name)
{
    set_widget_data(
        control.control_m.get(),
        kControlEntireControl,
        kControlStaticTextCFStringTag,
        explicit_cast<auto_cfstring_t>(name).get());
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(label_t, "<xstr id='metric:label'/>");

/****************************************************************************************************/

label_t::label_t(const std::string& name,
                 const std::string& alt_text,
                 std::size_t        characters,
                 theme_t            theme) :
    name_m(name),
    alt_text_m(alt_text),
    theme_m(theme),
    characters_m(characters)
{ }

/****************************************************************************************************/

void measure(label_t& value, extents_t& result)
{
    assert(value.control_m.get());

    ::Point          io_bounds = { 0 };
    ::SInt16         out_baseline(0);
    std::string      base_text(value.characters_m != 0 ?
                                std::string(value.characters_m * 2, std::string::value_type('0')) :
                                implementation::get_name(value));

    auto_cfstring_t auto_cfstring(explicit_cast<auto_cfstring_t>(base_text));
    
    ::ADOBE_REQUIRE_STATUS(::GetThemeTextDimensions(
        auto_cfstring.get(),
        implementation::theme_to_ThemeFontID(value.theme_m),
        kThemeStateActive,
        false,
        &io_bounds,
        &out_baseline));

    result.width() = io_bounds.h;

    result = implementation::apply_fudges(value, result);
}

/****************************************************************************************************/

void measure_vertical(label_t& value, extents_t& result, const place_data_t& placed_horizontal)
{
    // Note (fbrereto) : This is explicit (instead of using implementation::measure) because
    //                   we need to set the inbound rect to be the potential dimensions of the
    //                   text so the reflow will shrink the bounds if it needs to.

    assert(value.control_m.get());

    ::Rect           old_bounds = { 0 };

    ::GetControlBounds(value.control_m.get(), &old_bounds);

    ::Rect           static_bounds = { 0, 0, 2048, static_cast<short>(placed_horizontal.horizontal().length_m) };
    ::Rect           bounds = { 0 };
    ::SInt16         best_baseline(0);

    implementation::set_bounds(value.control_m, static_bounds);

    ::GetBestControlRect(value.control_m.get(), &bounds, &best_baseline);

    result.height() = bounds.bottom - bounds.top;
    result.width() = bounds.right - bounds.left;

    if (best_baseline)
        result.vertical().guide_set_m.push_back(result.height() + best_baseline);

    result = implementation::apply_fudges(value, result);

    implementation::set_bounds(value.control_m, old_bounds);
}

/****************************************************************************************************/

void place(label_t& value, const place_data_t& place_data)
{
    implementation::set_bounds(value, place_data);
}

/****************************************************************************************************/

void enable(label_t& value, bool make_enabled)
{
    ::ControlRef tmp(value.control_m.get());

    implementation::set_active(tmp, make_enabled);
}

/****************************************************************************************************/

void measure_label_text(const std::string& text, theme_t theme, extents_t& result)
{
    label_t label(text, std::string(), 0, theme);

    initialize(label, NULL);

    measure(label, result);

    place_data_t p;

    p.horizontal().length_m = result.width();

    measure_vertical(label, result, p);
}

/****************************************************************************************************/

template<>
platform_display_type insert<label_t>(display_t&             display,
                                             platform_display_type& parent,
                                             label_t&        element)
{
    ::HIViewRef parent_ref(parent);

    initialize(element, parent_ref);

    ::HIViewRef ref(element.control_m.get());

    platform_display_type result(display.insert(parent, ref));

    element.subevent_m.handler_m.install(ref);

    return result;
}              

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
