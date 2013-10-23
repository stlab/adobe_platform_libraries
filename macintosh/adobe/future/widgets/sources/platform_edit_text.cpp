/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_edit_text.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(edit_text_t, "<xstr id='metric:edit_text'/>");

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

template <>
inline dictionary_t extra_widget_context(const edit_text_t& w)
{
    static_name_t    is_scrollable(w.scrollable_m ? "true" : "false");
    dictionary_t     result;

    result.insert(std::make_pair(static_name_t("scroll"), any_regular_t(is_scrollable)));

    return result;
}

/****************************************************************************************************/

template <>
std::string get_name<edit_text_t>(const edit_text_t& widget)
{
    return get_name(widget.get_label());
}

/****************************************************************************************************/

template <>
std::string get_field_text<edit_text_t>(const edit_text_t& widget)
{
    if (!widget.control_m)
        return std::string();

    if (widget.scroll_control_m)
    {
        ::Handle text_handle(0);

        ::ADOBE_REQUIRE_STATUS(::TXNGetDataEncoded(::HITextViewGetTXNObject(widget.control_m), kTXNStartOffset, kTXNEndOffset, &text_handle, kTXNUnicodeTextData));

        auto_resource< ::Handle > auto_handle(text_handle);

        ::UniChar*  buffer(reinterpret_cast< ::UniChar* >(*text_handle));
        std::size_t size(::GetHandleSize(text_handle) / sizeof(::UniChar));

        return implementation::convert_utf(buffer, size);
    }
    else
    {
        ::CFStringRef cfstring(0);

        implementation::get_widget_data(widget.control_m, kControlEntireControl, kControlEditTextCFStringTag, cfstring);

        auto_cfstring_t auto_cfstring(cfstring);

        return explicit_cast<std::string>(auto_cfstring);
    }
}

/****************************************************************************************************/

template <>
void set_field_text<edit_text_t>(edit_text_t& widget, const std::string& text)
{
    assert(widget.control_m);

    widget.type_2_debounce_m = true;

    std::string             filtered(text);
    std::string::size_type  pos(0);

    while (true)
    {
        pos = filtered.find('\n', pos);

        if (pos == std::string::npos) break;

        filtered.replace(pos, 1, "\r");
    }

    if (widget.scrollable_m)
        ADOBE_REQUIRE_STATUS(::TXNSetData(::HITextViewGetTXNObject(widget.control_m), kTXNTextData,
                                            filtered.c_str(), filtered.size(), kTXNStartOffset, kTXNEndOffset));
    else
        implementation::set_widget_data(widget.control_m,
                        kControlEntireControl, kControlEditTextCFStringTag,
                        explicit_cast<auto_cfstring_t>(filtered).get());

    widget.type_2_debounce_m = false;
}

/****************************************************************************************************/

void get_selection(edit_text_t& edit_text,
                   std::size_t&        start_pos,
                   std::size_t&        end_pos)
{
    assert(edit_text.control_m);

    if (edit_text.scrollable_m)
    {
        // REVISIT (fbrereto) : Implement me
    }
    else
    {
        ::ControlEditTextSelectionRec selection = { 0 };

        get_widget_data(edit_text.control_m, kControlEditTextPart, kControlEditTextSelectionTag, selection);

        start_pos = selection.selStart;
        end_pos = selection.selEnd;
    }
}

/****************************************************************************************************/

template <>
void set_selection<edit_text_t>(edit_text_t& edit_text,
                                       std::size_t         start_pos,
                                       std::size_t         end_pos)
{
    assert(edit_text.control_m);

    if (edit_text.scrollable_m)
    {
        ::TXNObject textview(::HITextViewGetTXNObject(edit_text.control_m));

        implementation::set_selection(textview, start_pos, end_pos);
    }
    else
    {
        ::ControlEditTextSelectionRec selection =
        {
            static_cast< ::SInt16 >(start_pos),
            static_cast< ::SInt16 >(end_pos)
        };

        set_widget_data(edit_text.control_m, kControlEditTextPart, kControlEditTextSelectionTag, selection);
    }
}

/****************************************************************************************************/

template <>
void set_theme<edit_text_t>(edit_text_t& edit_text,
                                   theme_t      theme)
{
    if (edit_text.scrollable_m)
    {
        ::TXNObject textview(::HITextViewGetTXNObject(edit_text.control_m));

        implementation::set_theme(textview, theme);
    }
    else
        implementation::set_theme(edit_text.control_m, theme);
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

inline CGRect to_CGRect(const ::Rect& r)
{
    return ::CGRectMake(r.left,
                        r.top,
                        r.right - r.left,
                        r.bottom - r.top);
}

/****************************************************************************************************/

inline ::Rect to_Rect(const adobe::place_data_t& p)
{
    ::Rect result =
    {
        static_cast<short>(adobe::top(p)),
        static_cast<short>(adobe::left(p)),
        static_cast<short>(adobe::bottom(p)),
        static_cast<short>(adobe::right(p))
    };

    return result;
}

/****************************************************************************************************/

void initialize(adobe::edit_text_t& control, ::HIViewRef parent)
{
    typedef adobe::auto_resource< ::CFMutableDictionaryRef > auto_dict_t;

    static const ::Rect bounds_s = { 0, 0, 20, 100 };

    assert(!control.control_m);

    if (control.scrollable_m)
    {
        static const long    num_tags(3);
        ::TXNControlTag      tags[num_tags];
        ::TXNControlData     data[num_tags];
        ::TXNCarbonEventInfo event_info;
        auto_dict_t          the_dict(::CFDictionaryCreateMutable(NULL, 0, NULL, NULL));
        ::CGRect             cg_bounds(to_CGRect(bounds_s));

        tags[0] = kTXNWordWrapStateTag;
        data[0].uValue = kTXNNoAutoWrap;

        tags[1] = kTXNUseCarbonEvents;
        data[1].uValue = reinterpret_cast<UInt32>(&event_info);

        tags[2] = kTXNAutoIndentStateTag;
        data[2].uValue = kTXNAutoIndentOff;

        //if (lock) // REVISIT (fbrereto) : Add support for locking an edit text field from editing.
        //tags[3] = kTXNNoUserIOTag;
        //data[3].uValue = true;

        ::ADOBE_REQUIRE_STATUS(::HIScrollViewCreate(kHIScrollViewValidOptions, &control.scroll_control_m));

        ::HIViewSetFrame(control.scroll_control_m, &cg_bounds);

        ::HIViewSetVisible(control.scroll_control_m, true);

        ::ADOBE_REQUIRE_STATUS(::HITextViewCreate(NULL,
                                                  0,
                                                  kTXNMonostyledTextMask   |
                                                    kTXNWantHScrollBarMask |
                                                    kTXNWantVScrollBarMask,
                                                  &control.control_m));

        ::HIViewSetVisible(control.control_m, true);

        ::ADOBE_REQUIRE_STATUS(::HIViewAddSubview(control.scroll_control_m, control.control_m));

        event_info.useCarbonEvents = true;
        event_info.filler = 0;
        event_info.flags = kTXNNoAppleEventHandlersMask;
        event_info.fDictionary = the_dict.get();

        ::CFDictionaryAddValue(the_dict.get(), kTXNCommandTargetKey, ::GetControlEventTarget(control.control_m));

        ::ADOBE_REQUIRE_STATUS(::TXNSetTXNObjectControls(::HITextViewGetTXNObject(control.control_m),
                                                         false, num_tags, &tags[0], &data[0]));
    }
    else
    {
        ::ADOBE_REQUIRE_STATUS(::CreateEditUnicodeTextControl(::GetControlOwner(parent),
                                                              &bounds_s, NULL,
                                                              control.password_m,
                                                              NULL, &control.control_m));

        control.mouse_handler_m.handler_m.install(control.control_m);

        adobe::implementation::set_widget_data(
            control.control_m,
            kControlEditTextPart,
            kControlEditTextSingleLineTag,
            static_cast< ::Boolean >(control.rows_m <= 1));
    }

    adobe::implementation::set_theme(control.control_m, control.theme_m);

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);

    ::SetControlReference(control.control_m, reinterpret_cast< ::SInt32 >(&control));
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

::OSStatus edit_text_handler_t::handle_event(::EventHandlerCallRef next,
                                             ::EventRef            event)
{
    if (widget_m.type_2_debounce_m)
        return noErr;

    if (widget_m.pre_edit_proc_m.empty() && widget_m.post_edit_proc_m.empty())
        return eventNotHandledErr;

    // save off the old text and selection
    std::string old_text(implementation::get_field_text(widget_m));
    std::size_t selection_start(0);
    std::size_t selection_end(0);

    implementation::get_selection(widget_m, selection_start, selection_end);

    // pass the event to the control to fill in the text
    ::CallNextEventHandler(next, event);

    // get the new text from the control
    std::string new_text(implementation::get_field_text(widget_m));

    // if the text is the same, we're done
    if (old_text == new_text)
        return noErr;

    // send the new text off to the prefilter, but only
    // if we haven't hit our max length quota (but only
    // in the case when we're one row high)
    bool squelch(widget_m.rows_m == 1 &&
                 widget_m.max_cols_m > 0 &&
                 static_cast<std::size_t>(widget_m.max_cols_m) < new_text.size());

    if (squelch == false && widget_m.pre_edit_proc_m)
        widget_m.pre_edit_proc_m(new_text, squelch);

    if (squelch)
    {
        // if we squelch then we have to reset the text and the
        // selection back to what it was originally

        implementation::set_field_text(widget_m, old_text);

        implementation::set_selection(widget_m, selection_start, selection_end);

        ::AudioServicesPlayAlertSound(kUserPreferredAlert);
    }
    else
    {
        widget_m.type_1_debounce_m = new_text;

        if (widget_m.post_edit_proc_m)
        {
            // if we do not squelch pass the text on to the set_model_proc
    
            widget_m.post_edit_proc_m(new_text);
        }
    }

    return noErr;
}

/****************************************************************************************************/

edit_text_t::edit_text_t(const edit_text_ctor_block_t& block):
    control_m(0),
    scroll_control_m(0),
    name_m(new label_t(block.name_m, block.alt_text_m, 0, block.theme_m)),
    alt_text_m(block.alt_text_m),
    theme_m(block.theme_m),
    using_label_m(!block.name_m.empty()),
    scrollable_m(block.scrollable_m),
    password_m(block.password_m),
    rows_m(block.num_lines_m),
    cols_m(block.min_characters_m),
    max_cols_m(block.max_characters_m),
    static_width_m(0),
    static_height_m(0),
    static_baseline_m(0),
    edit_height_m(0),
    edit_baseline_m(0),
    edit_text_handler_m(*this)
{ }

/****************************************************************************************************/

void edit_text_t::measure(extents_t& result)
{
    assert(control_m);

    extents_t attrs;
    std::string      base_text(cols_m, std::string::value_type('0'));

    if (rows_m > 1)
    {
        attrs = implementation::measure_theme_text(base_text, theme_m);

        result = attrs;
        
        // REVISIT (fbrereto) : There is an issue here where the attributes of an edit_text
        //                      widget don't propagate properly to a static_text widget, so
        //                      measurement is inherently inaccurate. A better solution should
        //                      be found than simply doubling the row count.
        result.height() *= rows_m;
    }
    else
    {
        std::string placeholder(implementation::get_field_text(*this));

        implementation::set_field_text(*this, base_text);

        // measure the control directly because we don't apply the
        // fudges yet; we want to do that with the edit text values

        result = implementation::measure(this->control_m);

        implementation::set_field_text(*this, placeholder);
    }

    result = implementation::apply_fudges(*this, result);

    edit_height_m = result.height();
    edit_baseline_m = result.vertical().guide_set_m[0];

    if (!using_label_m)
        return;

    extents_t label_bounds(implementation::measure_theme_text(implementation::get_name(get_label()), theme_m));

    if (result.vertical().guide_set_m.size() && label_bounds.vertical().guide_set_m.size())
        align_slices(result.vertical(), label_bounds.vertical());

    result.horizontal() = implementation::merge_slices_with(result,
                                                                   metrics_m,
                                                                   label_bounds,
                                                                   get_label().metrics_m,
                                                                   extents_slices_t::horizontal);

    result.width() += implementation::global_metrics()(implementation::k_metric_gap);

    result.horizontal().guide_set_m.push_back(label_bounds.width());

    static_width_m = label_bounds.width();
    static_height_m = label_bounds.height();
    static_baseline_m = label_bounds.vertical().guide_set_m.empty() ? 0 : label_bounds.vertical().guide_set_m[0];
}

/****************************************************************************************************/

void edit_text_t::place(const place_data_t& place_data)
{
    using adobe::place;

    assert(control_m);

    place_data_t placement(place_data);
    long         baseline(place_data.vertical().guide_set_m.empty() ?
                          0 : place_data.vertical().guide_set_m[0]);

    if (using_label_m)
    {
        place_data_t label_placement;

        top(label_placement) = top(place_data);
        left(label_placement) = left(place_data);

        height(label_placement) = static_height_m;
        width(label_placement) = place_data.horizontal().guide_set_m[0];

        top(label_placement) += baseline - static_baseline_m;

        place(get_label(), label_placement);

        left(placement)  += width(label_placement) + implementation::global_metrics()(implementation::k_metric_gap);
        width(placement) -= width(label_placement) + implementation::global_metrics()(implementation::k_metric_gap);
    }

    top(placement) += baseline - edit_baseline_m;

    if (scrollable_m)
    {
        implementation::shed_fudges(*this, placement);

        implementation::set_bounds(scroll_control_m, to_Rect(placement));
    }
    else
    {
        implementation::set_bounds(*this, placement);
    }
}

/****************************************************************************************************/

void edit_text_t::enable(bool active)
{
    assert(control_m);

    implementation::set_active(control_m, active);
}

/****************************************************************************************************/

void edit_text_t::display(const model_type& text)
{
    if (text == type_1_debounce_m)
        return;

    type_1_debounce_m = text;

    implementation::set_field_text(*this, text);
}

/****************************************************************************************************/

void edit_text_t::set_selection(long start_char, long end_char)
{
    assert(control_m);

    implementation::set_selection(*this, start_char, end_char);
}

/****************************************************************************************************/
 
void edit_text_t::signal_pre_edit(const edit_text_pre_edit_proc_t& proc)
{
    pre_edit_proc_m = proc;
}

/****************************************************************************************************/

void edit_text_t::monitor(const setter_type& proc)
{
    post_edit_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<edit_text_t>(display_t&             display,
                                                 platform_display_type& parent,
                                                 edit_text_t&    element)
{
    ::HIViewRef parent_ref(parent);

    if (element.using_label_m)
    {
        insert(display, parent, element.get_label());
    }

    initialize(element, parent_ref);

    element.edit_text_handler_m.handler_m.install(element.control_m);

    return display.insert(parent, get_display(element));
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
