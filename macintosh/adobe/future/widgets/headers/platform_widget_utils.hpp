/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_UI_CORE_OS_UTILITIES_HPP
#define ADOBE_UI_CORE_OS_UTILITIES_HPP

/**************************************************************************************************/

#if !defined(__LP64__)

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <iterator>

#include <adobe/algorithm/set.hpp>
#include <adobe/dictionary_set.hpp>
#include <adobe/extents.hpp>
#include <adobe/future/macintosh_error.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/istream.hpp>
#include <adobe/macintosh_memory.hpp>
#include <adobe/macintosh_string.hpp>
#include <adobe/name.hpp>
#include <adobe/unicode.hpp>
#include <adobe/widget_attributes.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/table_index.hpp>

#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include <stdexcept>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

typedef ::WindowRef keyboard_handler_type;

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

typedef ::ControlRef platform_control_type;

/****************************************************************************************************/

extern aggregate_name_t     k_attribute_theme;
extern aggregate_name_t     k_attribute_theme_large;
extern aggregate_name_t     k_attribute_theme_normal;
extern aggregate_name_t     k_attribute_theme_small;
extern aggregate_name_t     k_attribute_theme_mini;

extern aggregate_name_t k_metric_gap;

extern aggregate_name_t k_metric_size;
extern aggregate_name_t k_metric_adjust_position;
extern aggregate_name_t k_metric_adjust_size;
extern aggregate_name_t k_metric_adjust_baseline;
extern aggregate_name_t k_metric_outset;
extern aggregate_name_t k_metric_frame;
extern aggregate_name_t k_metric_inset;
extern aggregate_name_t k_metric_spacing;

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename T>
bool get_widget_data(::ControlRef widget, ::ControlPartCode part, ::ResType tag, T& data, bool throwing = true)
{
    ::Size      t_size(sizeof(T));
    ::Size      actual_size(0);
    ::OSStatus  result(::GetControlData(widget, part, tag, t_size, &data, &actual_size));

    if (!throwing)
        return result == noErr;

    ::ADOBE_REQUIRE_STATUS(result);

    return t_size == actual_size;
}

/****************************************************************************************************/

inline unsigned long get_current_tick_time()
{
    return ::EventTimeToTicks(::GetLastUserEventTime());
}

/****************************************************************************************************/

template <typename Widget>
inline boost::uint32_t get_value(const Widget& widget)
{
    return get_value< ::ControlRef >(widget.control_m);
}

/****************************************************************************************************/

template <>
inline boost::uint32_t get_value< ::ControlRef >(const ::ControlRef& control)
{
    if (!control) return 0;

    return ::GetControl32BitValue(control);
}

/****************************************************************************************************/

template <typename T>
void get_bounds(const T& widget, ::Rect& bounds, bool absolute = false);

/****************************************************************************************************/

template <>
void get_bounds< ::ControlRef >(const ::ControlRef& control, ::Rect& bounds, bool absolute);

/****************************************************************************************************/

template <>
inline void get_bounds<auto_control_t>(const auto_control_t& control, ::Rect& bounds, bool absolute)
    { return get_bounds(control.get(), bounds, absolute); }

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

/// The field text is text that relates to (e.g. a popup) or is (e.g. an edit text) the value of the
/// widget. This is different from the label, as the field text corresponds directly to the value this
/// widget helps manage. If a widget has no field text (e.g. a checkbox) this function is undefined.

template <typename T>
std::string get_field_text(const T&);

/****************************************************************************************************/

template <>
std::string get_field_text< ::ControlRef >(const ::ControlRef& control);

/****************************************************************************************************/

template <>
inline std::string get_field_text<auto_control_t>(const auto_control_t& control)
    { return get_field_text(control.get()); }

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

/// The name is text that decorates the widget, but is not related to the value of the
/// widget itself. For widgets that have no extra-text decorations, this function is undefined.
/// Note that a static text widget is a name by definition.

template <typename Widget>
std::string get_name(const Widget& widget)
{
    return get_name(widget.control_m);
}

/****************************************************************************************************/

template <>
std::string get_name< ::ControlRef >(const ::ControlRef& control);

/****************************************************************************************************/

template <>
std::string get_name< ::WindowRef >(const ::WindowRef& window);

/****************************************************************************************************/

template <>
inline std::string get_name<auto_control_t>(const auto_control_t& control)
    { return get_name(control.get()); }

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

inline ::ATSUFontID get_atsui_font_id(const char* font_name, std::size_t name_length)
{
    ::ATSUFontID    atsui_font_id;

    ::ADOBE_REQUIRE_STATUS(::ATSUFindFontFromName(  font_name,
                                                    name_length,
                                                    static_cast<unsigned long>(kFontNoName),
                                                    kFontNoPlatformCode,
                                                    kFontNoScriptCode,
                                                    kFontNoLanguageCode,
                                                    &atsui_font_id));

    return atsui_font_id;
}

/****************************************************************************************************/

inline ::ATSUFontID get_atsui_font_id(const char* font_name)
{
    return get_atsui_font_id(font_name, std::strlen(font_name));
}

/****************************************************************************************************/

::FMFontFamily get_classic_font_id(const char* font_name, std::size_t name_length);

/****************************************************************************************************/

inline ::FMFontFamily get_classic_font_id(const char* font_name)
{
    return get_classic_font_id(font_name, std::strlen(font_name));
}

/****************************************************************************************************/

inline ::FMFontFamily get_classic_font_id(Str255 font_name)
{
    const char* first(reinterpret_cast<char*>(&font_name[1]));
    const char* last(first + font_name[0]);
    std::string font_name_c(first, last);

    return get_classic_font_id(font_name_c.c_str(), font_name_c.size());
}

/****************************************************************************************************/

inline ::ThemeFontID theme_to_ThemeFontID(const theme_t& style)
{
    theme_t  masked = static_cast<theme_t>(style & theme_mask_s);
    ::ThemeFontID   font_id(kThemeSystemFont);

    switch (masked)
    {
        case theme_mini_s:   font_id = kThemeMiniSystemFont;     break;
        case theme_small_s:  font_id = kThemeSmallSystemFont;    break;
        case theme_large_s:
        case theme_normal_s:
        default:                    font_id = kThemeSystemFont;         break;
    }

    return font_id;
}

/****************************************************************************************************/

template <typename T>
void atsu_set_attribute(::ATSUStyle style, ::ATSUAttributeTag tag, T value)
{
    ::ByteCount             size = sizeof(value);
    ::ATSUAttributeValuePtr ptr = &value;

    ADOBE_REQUIRE_STATUS(::ATSUSetAttributes(style, 1, &tag, &size, &ptr));
}

/****************************************************************************************************/

// this is using ATSUI on the backend, and should be replaced by measure_theme_text
// once we remove all the specializations for theme_t

extents_t get_text_dimensions(const std::string& text, theme_t theme);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

void theme_to_rec(const theme_t& style, ControlFontStyleRec& style_rec);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename T>
bool set_widget_data(::ControlRef widget, ::ControlPartCode part, ::ResType tag, const T& data, bool throwing = true)
{
    ::OSStatus  result(::SetControlData(widget, part, tag, sizeof(T), &data));

    if (!throwing)
        return result == noErr;

    ::ADOBE_REQUIRE_STATUS(result);

    return true;
}

/****************************************************************************************************/

void set_popup_with_text(ControlRef control, const std::string& text, unsigned long cur_time);

/****************************************************************************************************/

/// selects the text within a widget from [ start_pos, end_pos ] (note: inclusive end!)

template <typename T>
void set_selection(T& control, std::size_t start_pos, std::size_t end_pos);

/****************************************************************************************************/

template <>
inline void set_selection< ::TXNObject >(   ::TXNObject&    control,
                                            std::size_t     start_pos,
                                            std::size_t     end_pos)
{
    ::ADOBE_REQUIRE_STATUS(::TXNSetSelection(control, start_pos, end_pos));
}

/****************************************************************************************************/

template <typename Widget>
void set_focus(Widget& widget, bool make_focused)
{
    set_focus< ::ControlRef >(widget.control_m, make_focused);
}

/****************************************************************************************************/

template <>
void set_focus< ::ControlRef >(::ControlRef& control, bool make_focused);

/****************************************************************************************************/

template <typename T>
void set_refresh(const T& widget);

/****************************************************************************************************/

template <>
inline void set_refresh< ::ControlRef >(const ::ControlRef& control)
{
    assert(control);

    ADOBE_REQUIRE_STATUS(::HIViewSetNeedsDisplay(control, true));
}

/****************************************************************************************************/

template <>
inline void set_refresh<auto_control_t>(const auto_control_t& control)
    { set_refresh(control.get()); }

/****************************************************************************************************/

template <typename T>
void set_bounds(T& x, const ::Rect& new_bounds);

/****************************************************************************************************/

template <>
void set_bounds< ::ControlRef >(::ControlRef& control, const ::Rect& new_bounds);

/****************************************************************************************************/

template <>
inline void set_bounds<auto_control_t>(auto_control_t& control, const ::Rect& new_bounds)
    { ::ControlRef temp(control.get()); set_bounds(temp, new_bounds); }

/****************************************************************************************************/
#if 0
template <>
inline void set_bounds<const auto_control_t>(const auto_control_t& control, const ::Rect& new_bounds)
    { ::ControlRef temp(control.get()); set_bounds(temp, new_bounds); }
#endif
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename Widget>
inline const std::string& widget_tag(const Widget&)
{ static const std::string value_s("<xstr id='metric_unknown'/>{ }</xstr>"); return value_s; }

template <typename Widget>
inline dictionary_t extra_widget_context(const Widget&)
{ return dictionary_t(); }

#define ADOBE_WIDGET_TAG_BOILERPLATE(widgettype, static_tag)                        \
namespace implementation {                                                          \
template <> inline const std::string& widget_tag< widgettype >(const widgettype&)   \
{ static const std::string value_s(static_tag); return value_s; }                               \
} // namespace implementation

/****************************************************************************************************/

template <typename Widget>
void set_metric_extractor(Widget& w)
{
    // REVISIT (fbrereto) : If metrics can be changed at runtime for this
    // widget specifically this shortcut will have to go.
    if (!w.metrics_m.empty())
        return;

    name_t    attribute_theme_value(static_name_t("unknown"));
    dictionary_t     context;

    switch (w.theme_m & theme_mask_s)
    {
        case theme_large_s:  attribute_theme_value = k_attribute_theme_large;    break;
        case theme_normal_s: attribute_theme_value = k_attribute_theme_normal;   break;
        case theme_small_s:  attribute_theme_value = k_attribute_theme_small;    break;
        case theme_mini_s:   attribute_theme_value = k_attribute_theme_mini;     break;
        default:                    break;
    }
    
    context.insert(std::make_pair(k_attribute_theme, any_regular_t(attribute_theme_value)));

    w.metrics_m = metric_extractor_t(widget_metrics(widget_tag(w),
        dictionary_union(extra_widget_context(w), context)));
}

/****************************************************************************************************/

template <typename T>
inline void set_origin(T& x, float left, float top);

/****************************************************************************************************/
#if 0
template <>
inline void set_origin<const auto_control_t>(const auto_control_t& x, float left, float top)
{
    assert(false);
}
#endif
/****************************************************************************************************/

template <>
inline void set_origin< ::ControlRef >(::ControlRef& control, float left, float top)
    { ADOBE_REQUIRE_STATUS(::HIViewSetBoundsOrigin(control, left, top)); }

/****************************************************************************************************/

template <>
inline void set_origin<auto_control_t>(auto_control_t& control, float left, float top)
    { ::ControlRef temp(control.get()); set_origin(temp, left, top); }

/****************************************************************************************************/

template <typename Widget>
void shed_fudges(Widget& control, place_data_t& place_data)
{
    height(place_data) += control.metrics_m(k_metric_adjust_size, metric_extractor_t::index_height);
    width(place_data) += control.metrics_m(k_metric_adjust_size, metric_extractor_t::index_width);

    long fudge_left(control.metrics_m(k_metric_adjust_position, metric_extractor_t::index_left));
    long fudge_top(control.metrics_m(k_metric_adjust_position, metric_extractor_t::index_top));

    left(place_data) += fudge_left;
    top(place_data)  += fudge_top;

    /*
        NOTE (fbrereto) :   All Eve containers need to expand themselves to
        compensate for the case when their children have outsets. If
        this does not happen, you will see visual clipping of the
        children's visual outset artifacts.
    */

    float left_shift(fudge_left - place_data.horizontal().outset_m.first);
    float top_shift(fudge_top - place_data.vertical().outset_m.first);

    if (left_shift != 0 || top_shift != 0)
        set_origin(control.control_m, left_shift, top_shift);
}

/****************************************************************************************************/

template <typename Widget>
extents_t apply_fudges(const Widget& w, const extents_t& dimensions)
{
    extents_t result(dimensions);

    set_metric_extractor(w);

    result.height() -= w.metrics_m(k_metric_adjust_size, metric_extractor_t::index_height);
    result.width()  -= w.metrics_m(k_metric_adjust_size, metric_extractor_t::index_width);

    // frame inset and outsets are directly transcribed

    result.horizontal().outset_m.first += w.metrics_m(k_metric_outset, metric_extractor_t::index_left);
    result.horizontal().outset_m.second += w.metrics_m(k_metric_outset, metric_extractor_t::index_right);
    result.vertical().outset_m.first += w.metrics_m(k_metric_outset, metric_extractor_t::index_top);
    result.vertical().outset_m.second += w.metrics_m(k_metric_outset, metric_extractor_t::index_bottom);

    result.horizontal().frame_m.first += w.metrics_m(k_metric_frame, metric_extractor_t::index_left);
    result.horizontal().frame_m.second += w.metrics_m(k_metric_frame, metric_extractor_t::index_right);
    result.vertical().frame_m.first += w.metrics_m(k_metric_frame, metric_extractor_t::index_top);
    result.vertical().frame_m.second += w.metrics_m(k_metric_frame, metric_extractor_t::index_bottom);

    result.horizontal().inset_m.first += w.metrics_m(k_metric_inset, metric_extractor_t::index_left);
    result.horizontal().inset_m.second += w.metrics_m(k_metric_inset, metric_extractor_t::index_right);
    result.vertical().inset_m.first += w.metrics_m(k_metric_inset, metric_extractor_t::index_top);
    result.vertical().inset_m.second += w.metrics_m(k_metric_inset, metric_extractor_t::index_bottom);

    if (!result.vertical().guide_set_m.empty())
        // REVISIT (fbrereto) It will not always be safe to assume the first vertical poi is the baseline.
        result.vertical().guide_set_m[0] += w.metrics_m(implementation::k_metric_adjust_baseline);

    return result;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

extents_t::slice_t merge_slices_with(const extents_t&                 src_retangle1,
                                     const metric_extractor_t&        metrics1,
                                     const extents_t&                 src_retangle2,
                                     const metric_extractor_t&        metrics2,
                                     extents_slices_t::slice_select_t slice);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

extern aggregate_name_t k_metric_gap;

extern aggregate_name_t k_metric_size;
extern aggregate_name_t k_metric_adjust_position;
extern aggregate_name_t k_metric_adjust_size;
extern aggregate_name_t k_metric_adjust_baseline;
extern aggregate_name_t k_metric_outset;
extern aggregate_name_t k_metric_frame;
extern aggregate_name_t k_metric_inset;
extern aggregate_name_t k_metric_spacing;

const metric_extractor_t& global_metrics();

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename Widget>
extents_t measure(const Widget& widget)
    { return apply_fudges(widget, measure(widget.control_m)); }

/****************************************************************************************************/

template <>
extents_t measure< ::ControlRef >(const ::ControlRef& control);

/****************************************************************************************************/

template <>
inline extents_t measure<auto_control_t>(const auto_control_t& control)
    { return measure(control.get()); }

/****************************************************************************************************/

// looking for measure_label_text? Use this.
extents_t measure_theme_text(const std::string& text, theme_t theme);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename Widget>
void set_bounds(Widget& widget, const place_data_t& place_data)
{
    assert(widget.control_m);

    place_data_t placement(place_data);

    implementation::shed_fudges(widget, placement);

    ::Rect new_bounds =
    {
        static_cast<short>(top(placement)),
        static_cast<short>(left(placement)),
        static_cast<short>(bottom(placement)),
        static_cast<short>(right(placement))
    };

    set_bounds(widget.control_m, new_bounds);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

inline void set_name(::ControlRef& control, const std::string& name)
{
    assert(control);

    ADOBE_REQUIRE_STATUS(::SetControlTitleWithCFString(control, explicit_cast<auto_cfstring_t>(name).get()));
}

/****************************************************************************************************/

/// See comment in implementation::get_name about the differences between names and field texts

template <typename Widget>
void set_name(Widget& widget, const std::string& name)
{
    set_name(widget.control_m, name);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

/// See comment in implementation::get_field_text about the differences between names and field texts

template <typename T>
void set_field_text(T&, const std::string& name);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename Widget>
void set_theme(Widget& widget, theme_t theme)
{
    assert(widget.control_m);

    widget.theme_m = theme;

    set_theme(widget.control_m, theme);
}

/****************************************************************************************************/

template <>
void set_theme< ::ControlRef >(::ControlRef& control, theme_t theme);

/****************************************************************************************************/

template <>
void set_theme< ::TXNObject >(::TXNObject& hitextview, theme_t theme);

/****************************************************************************************************/

template <>
inline void set_theme<auto_control_t>(auto_control_t& control, theme_t theme)
    { ::ControlRef temp(control.get()); set_theme(temp, theme); }

/****************************************************************************************************/

template <typename Widget>
void adorn_theme(Widget& widget, theme_t theme)
{
    theme_t current(widget.theme_m);

    // wipe base from current if new base specified
    if (theme & theme_mask_s)
        current &= ~theme_mask_s; 

    current |= theme; // logical OR the two themes together

    set_theme(widget, current);
}

/****************************************************************************************************/

template <typename Widget>
void unadorn_theme(Widget& widget, theme_t theme)
{
    theme_t current(widget.theme_m);

    current &= ~theme;

    // If no base is specified now, default to normal
    if (!(current & theme_mask_s))
        current |= theme_normal_s;

    set_theme(widget, current);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename T>
void set_active(T&, bool make_active);

/****************************************************************************************************/

template <>
void set_active< ::ControlRef >(::ControlRef& control, bool make_active);

/****************************************************************************************************/

template <>
inline void set_active< auto_control_t >(auto_control_t& control, bool make_active)
    { ::ControlRef temp(control.get()); set_active(temp, make_active); }

/****************************************************************************************************/

template <typename T>
void set_visible(T&, bool make_visible);

/****************************************************************************************************/

template <>
inline void set_visible< ::ControlRef >(::ControlRef& control, bool make_visible)
{
    assert(control);

    ADOBE_REQUIRE_STATUS(::HIViewSetVisible(control, make_visible));
}

/****************************************************************************************************/

template <typename T>
void set_focused(T&, bool make_focused);

/****************************************************************************************************/

template <>
inline void set_focused< ::ControlRef >(::ControlRef& control, bool make_focused)
{
    assert(control);

    set_focus(control, make_focused);
}

/****************************************************************************************************/

template <typename T>
inline bool is_focused(T& control)
{ return is_focused(control.control_m); }

/****************************************************************************************************/

template <>
inline bool is_focused< ::ControlRef >(::ControlRef& control)
{
    assert(control);

    ::ControlRef cur_focus;

    ADOBE_REQUIRE_STATUS(::GetKeyboardFocus(::GetControlOwner(control), &cur_focus));

    return cur_focus == control;
}

template <>
inline bool is_focused< auto_control_t >(auto_control_t& control)
{ ::ControlRef temp(control.get()); return is_focused(temp); }

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

template <typename Widget>
extents_t widget_best_bounds(Widget& w, const std::string* first, const std::string* last)
{
    extents_t   result;
    int         text_height(0);
    int         text_width(0);
    int         text_baseline(-1);
    std::size_t longest_line_len(0);
    std::string placeholder(implementation::get_name(w));

    implementation::set_metric_extractor(w);

    for (; first != last; ++first)
    {
        longest_line_len = std::max(longest_line_len, first->size());

        implementation::set_name(w, *first);

        // we measure the control (not the widget) because we don't want the fudges applied yet

        extents_t    extents(implementation::measure(w.control_m));
        guide_set_t& poi(extents.vertical().guide_set_m);

        text_height = std::max(text_height, extents.height());
        text_width = std::max(text_width, extents.width());

        if (!poi.empty())
            text_baseline = std::max<int>(text_baseline, poi[0]);
    }

    result.width() = text_width;    
    result.height() = text_height;

    // we keep track of the longest_line_len to check and see if a non-blank
    // name was used. In that case do we then add the spacing metric.

    if (longest_line_len)
        result.width() += w.metrics_m(implementation::k_metric_spacing);
    else
    {
        // In the case when there was no name to speak of for the widget,
        // we still want it to baseline-align with its labelled siblings,
        // so we do some handwaving to get the correct baseline.

        implementation::set_name(w, std::string("X"));

        extents_t    extents(implementation::measure(w.control_m));
        guide_set_t& poi(extents.vertical().guide_set_m);

        if (!poi.empty())
            text_baseline = std::max<int>(text_baseline, poi[0]);
    }

    if (text_baseline != -1)
        result.vertical().guide_set_m.push_back(text_baseline);

    implementation::set_name(w, placeholder);

    return implementation::apply_fudges(w, result);
}

/****************************************************************************************************/

template <typename Widget>
inline extents_t widget_best_bounds(Widget& w, const std::string& name)
{
    const std::string* first(&name);

    return widget_best_bounds(w, first, first + 1);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

std::string convert_utf(::UniChar* buffer, std::size_t size);

/****************************************************************************************************/

std::string convert_utf(UniChar* buffer);

/****************************************************************************************************/

modifiers_t convert_modifiers(::UInt32 os_modifiers);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

bool context_menu(::WindowRef parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

boost::filesystem::path fsref_to_path(const ::FSRef& location);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

typedef boost::function<void (boost::uint32_t)> context_slider_callback_t;

void context_slider(boost::uint32_t left,
                    boost::uint32_t top,
                    boost::uint32_t width,
                    boost::uint32_t height,
                    boost::uint32_t value_min,
                    boost::uint32_t value_init,
                    boost::uint32_t value_max,
                    const context_slider_callback_t& proc);

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

class mouse_in_out_event_tag_t { } ;

struct mouse_in_out_event_handler_t
{
    mouse_in_out_event_handler_t()
    {
        handler_m.insert(kEventClassMouse, kEventMouseEntered);
        handler_m.insert(kEventClassMouse, kEventMouseExited);

        handler_m.monitor_proc_m = boost::bind(&mouse_in_out_event_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef,
                            ::EventRef event)
    {
        UInt32  event_class(::GetEventClass(event));
        UInt32  event_kind(::GetEventKind(event));

        if (event_class != kEventClassMouse)
            return eventNotHandledErr;

        if (event_kind == kEventMouseEntered)
            ADOBE_REQUIRE_STATUS(::SetThemeCursor(kThemeIBeamCursor));
        else if (event_kind == kEventMouseExited)
            ADOBE_REQUIRE_STATUS(::SetThemeCursor(kThemeArrowCursor));
        else
            return eventNotHandledErr;

        return noErr;
    }

    event_handler_t          handler_m;
    mouse_in_out_event_tag_t tag_m;
};

/****************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/****************************************************************************************************/

#endif

/****************************************************************************************************/
