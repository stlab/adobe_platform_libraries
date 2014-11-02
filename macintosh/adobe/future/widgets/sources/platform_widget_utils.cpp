/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <string>

#include <adobe/string.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/widgets/headers/edit_number.hpp>
#include <adobe/future/macintosh_events.hpp>

#include <boost/noncopyable.hpp>

using namespace std;

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

static_name_t k_attribute_theme = "theme"_name;
static_name_t k_attribute_theme_large = "large"_name;
static_name_t k_attribute_theme_normal = "normal"_name;
static_name_t k_attribute_theme_small = "small"_name;
static_name_t k_attribute_theme_mini = "mini"_name;

/****************************************************************************************************/

template <>
void get_bounds< ::ControlRef >(const ::ControlRef& control, ::Rect& bounds, bool absolute)
{
    assert(control);

    static const float  round(.5);
    ::HIRect            cur_bounds = { { 0, 0 }, { 0, 0 } };

    ADOBE_REQUIRE_STATUS(::HIViewGetFrame(control, &cur_bounds));

    if (absolute)
    {
        // bounds are relative to the parent control; this makes them relative to the window.

        ::WindowRef     window(::GetControlOwner(control));
        ::ControlRef    root_control(0);

        if (window)
            ::ADOBE_REQUIRE_STATUS(::GetRootControl(window, &root_control));

        if (root_control)
            ::ADOBE_REQUIRE_STATUS(::HIViewConvertRect(&cur_bounds, ::HIViewGetSuperview(control), root_control));
    }

    bounds.left     = static_cast<short>(cur_bounds.origin.x + round);
    bounds.top      = static_cast<short>(cur_bounds.origin.y + round);
    bounds.right    = static_cast<short>(bounds.left + cur_bounds.size.width + round);
    bounds.bottom   = static_cast<short>(bounds.top + cur_bounds.size.height + round);
}

/****************************************************************************************************/

template <>
std::string get_field_text< ::ControlRef >(const ::ControlRef& control)
{
    if (!control) return std::string();

    ::CFStringRef cfstring(0);

    ADOBE_REQUIRE_STATUS(::CopyControlTitleAsCFString(control, &cfstring));

    auto_cfstring_t auto_string(cfstring);

    return explicit_cast<std::string>(cfstring);
}

/****************************************************************************************************/

template <>
std::string get_name< ::ControlRef >(const ::ControlRef& control)
{
    assert (control);

    ::CFStringRef cfstring(0);

    ADOBE_REQUIRE_STATUS(::CopyControlTitleAsCFString(control, &cfstring));

    auto_cfstring_t auto_string(cfstring);

    return explicit_cast<std::string>(cfstring);
}

/****************************************************************************************************/

template <>
std::string get_name< ::WindowRef >(const ::WindowRef& window)
{
    assert(window);

    ::CFStringRef cfstring(0);

    ADOBE_REQUIRE_STATUS(::CopyWindowTitleAsCFString(window, &cfstring));

    auto_cfstring_t auto_string(cfstring);

    return explicit_cast<std::string>(cfstring);
}

/****************************************************************************************************/

/// REVISIT (fbrereto) : This is tucked into a cpp file because it uses a deprecated MacOS X API

#if 0
::FMFontFamily get_classic_font_id(const char* font_name, std::size_t name_length)
{
    ::ATSUFontID    atsui_font_id(get_atsui_font_id(font_name, name_length));
    ::FMFont        font(atsui_font_id);
//    ::FMFont        font(::FMGetFontFromATSFontRef(atsui_font_id));
    ::FMFontFamily  font_family;
    ::FMFontStyle   font_style;

    ADOBE_REQUIRE_STATUS(::FMGetFontFamilyInstanceFromFont(font, &font_family, &font_style));

    return font_family;
}
#endif

/****************************************************************************************************/

extents_t get_text_dimensions(const std::string& text, theme_t theme)
{
    extents_t                result;
    ::ATSUStyle                     style_nonauto;
    ::ATSUTextLayout                layout_nonauto;
    std::vector<boost::uint16_t>    unibuffer;
    ::UniCharCount                  run_length(kATSUToTextEnd);

    unibuffer.reserve(text.size());

    copy_utf<boost::uint16_t>(text.begin(), text.end(), std::back_inserter(unibuffer));

    ADOBE_REQUIRE_STATUS(::ATSUCreateStyle(&style_nonauto));

    auto_resource< ::ATSUStyle > style(style_nonauto);

    theme_t masked(theme & theme_mask_s);

    if (masked == theme_mini_s)
        atsu_set_attribute(style_nonauto, kATSUSizeTag, ::Long2Fix(9));
    else if (masked == theme_small_s)
        atsu_set_attribute(style_nonauto, kATSUSizeTag, ::Long2Fix(11));
    //else if (masked == theme_normal_s)
    //  atsu_set_attribute(style_nonauto, kATSUSizeTag, ::Long2Fix(13));

    if (theme & theme_adornment_mono_s)
        atsu_set_attribute(style_nonauto, kATSUFontTag, get_atsui_font_id("Monaco"));

    ADOBE_REQUIRE_STATUS(::ATSUCreateTextLayoutWithTextPtr(
        static_cast<UniChar*>(&unibuffer[0]),
        0,
        unibuffer.size(),
        unibuffer.size(),
        1,
        &run_length,
        &style_nonauto,
        &layout_nonauto));

    auto_resource< ::ATSUTextLayout > layout(layout_nonauto);

    ADOBE_REQUIRE_STATUS(::ATSUSetTransientFontMatching(layout_nonauto, true));

    ATSUTextMeasurement before(0);
    ATSUTextMeasurement after(0);
    ATSUTextMeasurement ascent(0);
    ATSUTextMeasurement descent(0);

    ADOBE_REQUIRE_STATUS(::ATSUGetUnjustifiedBounds(
        layout_nonauto,
        0,
        unibuffer.size(),
        &before,
        &after,
        &ascent,
        &descent));

    before = ::Fix2Long(before);
    after = ::Fix2Long(after);
    ascent = ::Fix2Long(ascent);
    descent = ::Fix2Long(descent);

    result.vertical().length_m = ascent;
    result.horizontal().length_m = after - before;
    result.vertical().guide_set_m.push_back(ascent);

    return result;
}

/****************************************************************************************************/

extents_t::slice_t merge_slices_with(const extents_t&                 src_retangle1,
                                            const metric_extractor_t&        metrics1,
                                            const extents_t&                 src_retangle2,
                                            const metric_extractor_t&        metrics2,
                                            extents_slices_t::slice_select_t slice)
{
    const extents_t::slice_t&            src1(src_retangle1.slice_m[slice]);
    const extents_t::slice_t&            src2(src_retangle2.slice_m[slice]);
    metric_extractor_t::array_index_t    yon(    slice == extents_slices_t::horizontal ?
                                                        metric_extractor_t::index_right :
                                                        metric_extractor_t::index_bottom);
    extents_t::slice_t                   dst(src1);

    dst.length_m += src2.length_m;

    dst.length_m += metrics1(implementation::k_metric_outset, yon);
    dst.outset_m.second += metrics1(implementation::k_metric_outset, yon);
    dst.outset_m.second -= metrics2(implementation::k_metric_outset, yon);

    dst.length_m += dst.inset_m.second;
    dst.inset_m.second = src2.inset_m.second;

    dst.length_m += dst.frame_m.second;
    dst.frame_m.second = src2.frame_m.second;

    return dst;
}

/****************************************************************************************************/

static_name_t k_metric_gap = "gap"_name;

static_name_t k_metric_size = "size"_name;
static_name_t k_metric_adjust_position = "adjust_position"_name;
static_name_t k_metric_adjust_size = "adjust_size"_name;
static_name_t k_metric_adjust_baseline = "adjust_baseline"_name;
static_name_t k_metric_outset = "outset"_name;
static_name_t k_metric_frame = "frame"_name;
static_name_t k_metric_inset = "inset"_name;
static_name_t k_metric_spacing = "spacing"_name;

/****************************************************************************************************/

const metric_extractor_t& global_metrics()
{
    static metric_extractor_t metrics_s(widget_metrics("<xstr id='metric:global'/>"));

    return metrics_s;
}

/****************************************************************************************************/

template<>
extents_t measure< ::ControlRef >(const ::ControlRef& control)
{
    assert(control);

    extents_t    result;
    ::Rect              bounds = { 0 };
    ::SInt16            best_baseline(0);

    ::GetBestControlRect(control, &bounds, &best_baseline);

    result.height() = bounds.bottom - bounds.top;
    result.width() = bounds.right - bounds.left;

    if (best_baseline)
        result.vertical().guide_set_m.push_back(result.height() + best_baseline);

    return result;
}

/****************************************************************************************************/

extents_t measure_theme_text(const std::string& text, theme_t theme)
{
    extents_t result;
    ::Point          io_bounds = { 0 };
    ::SInt16         out_baseline(0);

    auto_cfstring_t auto_cfstring(explicit_cast<auto_cfstring_t>(text));

    ::ADOBE_REQUIRE_STATUS(::GetThemeTextDimensions(
        auto_cfstring.get(),
        implementation::theme_to_ThemeFontID(theme),
        kThemeStateActive,
        false,
        &io_bounds,
        &out_baseline));

    result.width() = io_bounds.h;
    result.height() = io_bounds.v;

    result.vertical().guide_set_m.push_back(io_bounds.v + out_baseline);

    return result;
}

/****************************************************************************************************/

void theme_to_rec(const theme_t& style, ControlFontStyleRec& style_rec)
{
    ControlFontStyleRec result = { 0 };
    theme_t      masked(style & theme_mask_s);

    if (style & theme_adornment_mono_s)
    {
        assert(false && "Unclear how to get monospaced theme font.");
#if 0
        result.font = get_classic_font_id("Monaco");

        result.flags |= static_cast<int>(kControlUseFontMask) |
                        static_cast<int>(kControlUseSizeMask);

        switch (masked)
        {
            case theme_large_s:  result.size = 16;   break;
            case theme_small_s:  result.size = 12;   break;
            case theme_mini_s:   result.size = 11;   break;
            case theme_normal_s:
            default:                    result.size = 14;   break;
        }
#endif
    }
    else if (masked)
    {
        result.flags = static_cast<int>(kControlUseThemeFontIDMask);
        result.font = theme_to_ThemeFontID(style);
    }

    style_rec = result;
}

/****************************************************************************************************/

// REVISIT (fbrereto) : This can all get reduced

void set_popup_with_text(ControlRef control, const std::string& text, unsigned long cur_time)
{
    static unsigned long    last_time(0);
    static unsigned long    double_click_time(::GetDblTime());
    static std::string      selection_string;
    std::size_t             new_index(1);
    MenuRef                 menu(::GetControlPopupMenuHandle(control));
    std::size_t             menu_item_count(::CountMenuItems(menu));

    if (cur_time - last_time > double_click_time)
        selection_string.erase(selection_string.begin(), selection_string.end());

    last_time = cur_time;

    selection_string.append(text.c_str());

    for (; new_index <= menu_item_count; ++new_index)
    {
        ::CFStringRef cur_menu_item(0);

        ADOBE_REQUIRE_STATUS(::CopyMenuItemTextAsCFString(menu, static_cast< ::UInt16 >(new_index), &cur_menu_item));

        auto_cfstring_t  auto_string(cur_menu_item);
        std::string      cur_name(explicit_cast<std::string>(cur_menu_item));

        for (std::string::iterator i(cur_name.begin()); i != cur_name.end(); ++i)
            *i = static_cast<std::string::value_type>(std::tolower(*i));

        if (cur_name.find(selection_string) == 0) break;
    }

    if (new_index > menu_item_count)
        system_beep();
    else if (new_index != static_cast<std::size_t>(get_value(control)))
        ::SetControl32BitValue(control, static_cast<long>(new_index));
}

/****************************************************************************************************/

/// sets the keyboard focus for a control

template <>
void set_focus< ::ControlRef >(::ControlRef& control, bool make_focused)
{
    assert(control);

    ::ControlRef    current_focus(0);
    ::WindowRef     window(::GetControlOwner(control));

    ADOBE_REQUIRE_STATUS(::GetKeyboardFocus(window, &current_focus));

    if (make_focused && current_focus != control)
        ADOBE_REQUIRE_STATUS(::SetKeyboardFocus(window, control, kControlFocusNextPart));
    else if (!make_focused && current_focus == control)
        ADOBE_REQUIRE_STATUS(::ClearKeyboardFocus(window));
}

/****************************************************************************************************/

template<>
void set_bounds< ::ControlRef >(::ControlRef& control, const ::Rect& new_bounds)
{
    assert(control);

    ::HIRect bounds =
    {
        {   static_cast<float>(new_bounds.left),
            static_cast<float>(new_bounds.top)
        },
        {
            static_cast<float>(new_bounds.right - new_bounds.left),
            static_cast<float>(new_bounds.bottom - new_bounds.top)
        }
    };

    bounds.size.height = std::max(bounds.size.height, 0.0f);
    bounds.size.width = std::max(bounds.size.width, 0.0f);

    ADOBE_REQUIRE_STATUS(::HIViewSetFrame(control, &bounds));
}

/****************************************************************************************************/

void set_control_alt_text(::ControlRef control, const std::string& alt_text)
{
    static const ::Rect    empty_rect = { 0 };
    auto_cfstring_t        auto_cfstring;
    ::HMHelpContentRec     help_tag = { 0 };

    help_tag.version = kMacHelpVersion;
    help_tag.tagSide = kHMDefaultSide;
    help_tag.absHotRect = empty_rect;
    help_tag.content[kHMMaximumContentIndex].contentType = kHMNoContent;

    if (!alt_text.empty())
    {
        auto_cfstring = explicit_cast<auto_cfstring_t>(alt_text);
        help_tag.content[kHMMinimumContentIndex].contentType = kHMCFStringLocalizedContent;
        help_tag.content[kHMMinimumContentIndex].u.tagCFString = auto_cfstring.get();
    }

    ::ADOBE_REQUIRE_STATUS(::HMSetControlHelpContent(control, &help_tag));
}

/****************************************************************************************************/

template <>
void set_theme< ::ControlRef >(::ControlRef& control, theme_t theme)
{
    assert(control);

    ::ControlFontStyleRec new_style = { 0 };

    theme_to_rec(theme, new_style);

    set_widget_data(control, kControlEntireControl, kControlFontStyleTag, new_style, false);

    ::ControlSize control_size(kControlSizeNormal);

    switch ( theme & theme_mask_s )
    {
        case theme_large_s:     control_size = kControlSizeLarge;   break;
        case theme_small_s:     control_size = kControlSizeSmall;   break;
        case theme_mini_s:      control_size = kControlSizeMini;    break;
        case theme_normal_s:
        default:                break;
    }

    set_widget_data(control, kControlEntireControl, kControlSizeTag, control_size, false);
}

/****************************************************************************************************/

template <>
void set_theme< ::TXNObject >(::TXNObject& hitextview, theme_t theme)
{
    assert (hitextview);

    ControlFontStyleRec             rec;
    std::vector<TXNTypeAttributes>  attr;
    TXNTypeAttributes               empty_attr = { 0 };
    RGBColor                        black = { 0, 0, 0 };

    theme_to_rec(theme, rec);

    if (rec.flags & kControlUseFontMask)
    {
        attr.push_back(empty_attr);
        attr.back().tag = kTXNQDFontFamilyIDAttribute;
        attr.back().size = kTXNQDFontFamilyIDAttributeSize;
        attr.back().data.dataValue = rec.font;
    }

    if (rec.flags & kControlUseSizeMask)
    {
        attr.push_back(empty_attr);
        attr.back().tag = kTXNQDFontSizeAttribute;
        attr.back().size = kTXNFontSizeAttributeSize;
        attr.back().data.dataValue = static_cast<UInt32>(::Long2Fix(rec.size));
    }

    if (rec.flags & kControlUseFaceMask)
    {
        attr.push_back(empty_attr);
        attr.back().tag = kTXNQDFontStyleAttribute;
        attr.back().size = kTXNQDFontStyleAttributeSize;
        attr.back().data.dataValue = rec.style;
    }

    attr.push_back(empty_attr);
    attr.back().tag = kTXNQDFontColorAttribute;
    attr.back().size = kTXNQDFontColorAttributeSize;
    attr.back().data.dataPtr = &black;

    ::ADOBE_REQUIRE_STATUS(::TXNSetTypeAttributes(hitextview, attr.size(), &attr[0], kTXNStartOffset, kTXNEndOffset));
}

/****************************************************************************************************/

template <>
void set_active< ::ControlRef >(::ControlRef& control, bool make_active)
{
    assert(control);

    make_active ?
        ::EnableControl(control)
    :
        ::DisableControl(control);
}

/****************************************************************************************************/

std::string convert_utf(::UniChar* buffer, std::size_t size)
{
    std::string result;

    result.reserve(size * 2);

    copy_utf<char>(buffer, buffer + size, std::back_inserter(result));

    return result;
}

/****************************************************************************************************/

std::string convert_utf(UniChar* buffer)
{
    std::size_t size(0);

    while (buffer[size] != ::UniChar(0)) ++size;

    return convert_utf(buffer, size);
}

/****************************************************************************************************/

modifiers_t convert_modifiers(::UInt32 os_modifiers)
{
    modifiers_t result(modifiers_none_s);

    if (os_modifiers & cmdKey)          result |= modifiers_any_command_s;
    if (os_modifiers & shiftKey)        result |= modifiers_left_shift_s;
    if (os_modifiers & alphaLock)       result |= modifiers_caps_lock_s;
    if (os_modifiers & optionKey)       result |= modifiers_left_option_s;
    if (os_modifiers & controlKey)      result |= modifiers_left_control_s;
    if (os_modifiers & rightShiftKey)   result |= modifiers_right_shift_s;
    if (os_modifiers & rightOptionKey)  result |= modifiers_right_option_s;
    if (os_modifiers & rightControlKey) result |= modifiers_right_control_s;

    return result;
}

/****************************************************************************************************/

bool context_menu(::WindowRef parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result)
{
    ::MenuRef            menu(0);
    const name_t* saved_first(first);
    ::ControlRef         focus_control(0);

    ADOBE_REQUIRE_STATUS(::CreateNewMenu(0, kMenuAttrCondenseSeparators, &menu));

    auto_resource< ::MenuRef > auto_menu(menu);

    for (; first != last; ++first)
        ::AppendMenuItemTextWithCFString(menu, explicit_cast<auto_cfstring_t>(string(first->c_str())).get(), 0, 0, 0);

    ::GetKeyboardFocus(parent, &focus_control);
    ::SetKeyboardFocus(parent, focus_control, kControlFocusNoPart);

    long popup_result(::PopUpMenuSelect(menu, y, x, 0));

    ::SetKeyboardFocus(parent, focus_control, kControlFocusNextPart);

    std::size_t selection(popup_result & 0xffff);

    if (selection == 0)
        return false;

    result = name_t(*(saved_first + selection - 1));

    return true;
}

/****************************************************************************************************/

//
/// Convert a MacOS FSRef into a Boost path. This allows us to use the
/// boost::filesystem library to perform operations on the path, rather
/// than using the FSRef API.
///
/// \param  location    the FSRef to make into a bfs::path
/// \return a bfs path of the given location.
//
boost::filesystem::path fsref_to_path( const ::FSRef& location )
{
    std::size_t         max_size (1024);
    std::vector<char>   path_buffer(max_size);
    OSStatus            error(noErr);
    
    while (true)
    {
        error = ::FSRefMakePath(&location, reinterpret_cast<UInt8*>(&path_buffer[0]), max_size);
        if (error != pathTooLongErr) break;
        
        max_size *= 2;
        path_buffer.resize(max_size);
    }
    
    ADOBE_REQUIRE_STATUS(error);
    
    return boost::filesystem::path( &path_buffer[0]);
}

/****************************************************************************************************/

bool pick_file(boost::filesystem::path& path, platform_display_type /*dialog_parent*/)
{
    ::NavDialogRef             nav_dialog;
    ::NavDialogCreationOptions nav_options;

    ::NavGetDefaultDialogCreationOptions(&nav_options);

    ::NavCreateChooseFileDialog(&nav_options,
                                0,
                                0,
                                0,
                                0,
                                0,
                                &nav_dialog);

    auto_resource< ::NavDialogRef > auto_nav_dialog(nav_dialog);

    ::NavDialogRun(nav_dialog);

    ::NavUserAction user_action = ::NavDialogGetUserAction(nav_dialog);

    if (user_action == kNavUserActionCancel || user_action == kNavUserActionNone)
        return false;

    ::NavReplyRecord reply;

    ::NavDialogGetReply(nav_dialog, &reply);

    auto_resource< ::NavReplyRecord* > auto_reply(&reply);

    if (reply.validRecord == false)
        return false;

    // filter out the answer from the reply

    ::FSRef     file;
    ::Size      actual_size;
    ::AEKeyword keyword;
    ::DescType  type_code;

    ::AEGetNthPtr(  &reply.selection, 1, typeFSRef, &keyword, &type_code,
                    (Ptr) &file, sizeof(file), (actual_size = sizeof(file), &actual_size));

    path = fsref_to_path(file);

    path /= explicit_cast<std::string>(reply.saveFileName);

    return true;
}

/****************************************************************************************************/

bool pick_save_path(boost::filesystem::path& path, platform_display_type /*dialog_parent*/)
{
    ::NavDialogRef             nav_dialog;
    ::NavDialogCreationOptions nav_options;

    ::NavGetDefaultDialogCreationOptions(&nav_options);

    ::NavCreatePutFileDialog(&nav_options,
                             OSType('utf8'),
                             OSType('R*ch'), // BBEdit
                             0,
                             0,
                             &nav_dialog);

    auto_resource< ::NavDialogRef > auto_nav_dialog(nav_dialog);

    ::NavDialogRun(nav_dialog);

    ::NavUserAction user_action = ::NavDialogGetUserAction(nav_dialog);

    if (user_action == kNavUserActionCancel || user_action == kNavUserActionNone)
        return false;

    ::NavReplyRecord reply;

    ::NavDialogGetReply(nav_dialog, &reply);

    auto_resource< ::NavReplyRecord* > auto_reply(&reply);

    if (reply.validRecord == false)
        return false;

    // filter out the answer from the reply

    ::FSRef     file;
    ::Size      actual_size;
    ::AEKeyword keyword;
    ::DescType  type_code;

    ::AEGetNthPtr(  &reply.selection, 1, typeFSRef, &keyword, &type_code,
                    (Ptr) &file, sizeof(file), (actual_size = sizeof(file), &actual_size));

    path = fsref_to_path(file);

    path /= explicit_cast<std::string>(reply.saveFileName);

    return true;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

pascal void context_slider_action(ControlRef control, ControlPartCode)
try
{
    context_slider_callback_t& slider_callback(*reinterpret_cast<context_slider_callback_t*>(::GetControlReference(control)));

    if (slider_callback)
        slider_callback(static_cast<boost::uint32_t>(::GetControl32BitValue(control)));
}
catch (const stream_error_t& err)
{
    report_error(format_stream_error(err));
}
catch (const std::exception& err)
{
    report_error(std::string("Exception: ") + err.what());
}
catch (...)
{
    report_error("Unknown Error");
}

/****************************************************************************************************/

struct context_slider_deactivate_handler_t
{
    explicit context_slider_deactivate_handler_t(::WindowRef& window) :
        window_m(window)
    {
        handler_m.insert(kEventClassMouse, kEventMouseDown);

        handler_m.monitor_proc_m = boost::bind(&context_slider_deactivate_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef /*next*/,
                            ::EventRef            event)
    {
        ::WindowRef window_under(0);

        if (get_event_parameter<kEventParamWindowRef>(event, window_under) && window_under != window_m)
        {
            ::QuitAppModalLoopForWindow(window_m);

            return noErr;
        }

        return eventNotHandledErr;
    }

    event_handler_t handler_m;
    ::WindowRef     window_m;
};

/****************************************************************************************************/

void context_slider(boost::uint32_t left,
                    boost::uint32_t top,
                    boost::uint32_t width,
                    boost::uint32_t height,
                    boost::uint32_t value_min,
                    boost::uint32_t value_init,
                    boost::uint32_t value_max,
                    const context_slider_callback_t& proc)
{
    static const ::ControlActionUPP       slider_handler(::NewControlActionUPP(context_slider_action));
    ::WindowRef                           window(0);
    ::ControlRef                          root_control(0);
    ::ControlRef                          slider(0);
    ::Rect                                bounds = { static_cast<short>(top), static_cast<short>(left), static_cast<short>(top + height), static_cast<short>(left + width) };
    context_slider_callback_t             callback(proc);

    ADOBE_REQUIRE_STATUS(::CreateNewWindow(kHelpWindowClass, kWindowStandardHandlerAttribute | kWindowCompositingAttribute, &bounds, &window));

    context_slider_deactivate_handler_t deactivate_handler(window);

    long margin(3);

    bounds.top = margin;
    bounds.left = margin;
    bounds.bottom = height - margin;
    bounds.right = width - margin;

    ADOBE_REQUIRE_STATUS(::SetWindowAlpha(window, .6));

    ADOBE_REQUIRE_STATUS(::GetRootControl(window, &root_control));
    ADOBE_REQUIRE_STATUS(::CreateSliderControl( window, &bounds,
                                                value_init, value_min, value_max,
                                                kControlSliderDoesNotPoint,
                                                0, true,
                                                slider_handler,
                                                &slider));

    set_widget_data(slider, kControlEntireControl, kControlSizeTag, ::ControlSize(kControlSizeMini), false);

    ::SetControlReference(slider, reinterpret_cast< ::SInt32 >(&callback));

    ::ShowWindow(window);
    ::ActivateWindow(window, true);
    ::DrawControls(window);

    deactivate_handler.handler_m.install(::GetEventDispatcherTarget());

    ::RunAppModalLoopForWindow(window);

    ::DisposeWindow(window);
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

platform_display_type get_top_level_window(platform_display_type thing)
{
    return ::HIViewGetRoot(::GetControlOwner(thing));
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
