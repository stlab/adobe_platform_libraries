/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_tab_group.hpp>

#include <adobe/algorithm/find_match.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/macintosh_memory.hpp>
#include <adobe/macintosh_string.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

void initialize(adobe::tab_group_t& control, ::HIViewRef parent)
{
    assert (!control.control_m);

    static const ::Rect bounds_s = {0, 0, 10, 10 };

    ::ControlTabSize      size(kControlSizeNormal);
    ::ControlTabDirection direction(kControlTabDirectionNorth);
    long                  group_value(0);

    ADOBE_REQUIRE_STATUS(::CreateTabsControl(::GetControlOwner(parent),
                                             &bounds_s,
                                             size, direction,
                                             0, NULL, &control.control_m));

    adobe::implementation::set_theme(control, control.theme_m);

    // we have to set the max value of the tab group before we set any tab values
    ::SetControl32BitMaximum(control.control_m, control.items_m.size());

    for (adobe::tab_group_t::tab_set_t::iterator first(control.items_m.begin()), last(control.items_m.end());
         first != last; ++first)
    {
        adobe::auto_cfstring_t cfstring(adobe::explicit_cast<adobe::auto_cfstring_t>(first->name_m));

        ::ControlTabInfoRecV1 new_tab =
        {
            kControlTabInfoVersionOne,
            0, cfstring.get()
        };

        adobe::implementation::set_widget_data(control.control_m, ++group_value, kControlTabInfoTag, new_tab);
    }
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(tab_group_t, "<xstr id='metric:tab_group'/>");

/****************************************************************************************************/

::OSStatus tab_group_value_handler_t::handle_event( ::EventHandlerCallRef   next,
                                                    ::EventRef              event)
{
    OSStatus result(::CallNextEventHandler(next, event)); // Handle us last

    if (setter_m.empty() || widget_m.debounce_m)
        return result;

    widget_m.debounce_m = true;

    std::size_t value = implementation::get_value(widget_m);
    if (value != 0) --value;

    setter_m(widget_m.items_m[value].value_m);

    widget_m.debounce_m = false;

    return result;
}

/****************************************************************************************************/

tab_group_t::tab_group_t(const tab_t* first, const tab_t* last, theme_t theme) :
    control_m(NULL),
    theme_m(theme),
    items_m(first, last),
    value_handler_m(*this),
    debounce_m(false)
{
}

/****************************************************************************************************/

void tab_group_t::measure(extents_t& result)
{
    // On the Mac OS, tab groups don't have a "name", nor a corresponding baseline, either,
    // so we need to fetch the lengths manually

    implementation::set_metric_extractor(*this);

    for (tab_set_t::iterator first(items_m.begin()), last(items_m.end()); first != last; ++first)
    {
        extents_t cur_extents;

		measure_label_text(first->name_m, theme_m, cur_extents);

        result.width() += cur_extents.width() + metrics_m(implementation::k_metric_spacing);

        result.vertical().frame_m.first = std::max(result.vertical().frame_m.first, cur_extents.height());
    }

    result = implementation::apply_fudges(*this, result);
}

/****************************************************************************************************/

void tab_group_t::place(const place_data_t& place_data)
{
    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void tab_group_t::display(const any_regular_t& new_value)
{
    assert(control_m);

    if (debounce_m)
        return;

    debounce_m = true;

    tab_set_t::iterator iter(find_match(
        items_m,
        new_value,
        compare_members(&tab_t::value_m, std::equal_to<any_regular_t>())));
    
    if (iter != items_m.end())
        ::SetControl32BitValue(control_m, (iter - items_m.begin()) + 1);

    debounce_m = false;
}

/****************************************************************************************************/

void tab_group_t::monitor(const tab_group_value_proc_t& proc)
{
    assert(control_m);

    value_handler_m.setter_m = proc;

    value_handler_m.handler_m.install(control_m);
}

/****************************************************************************************************/

template <>
platform_display_type insert<tab_group_t>(display_t&             display,
                                                platform_display_type&  parent,
                                                tab_group_t&     element)
{
    ::HIViewRef parent_ref(parent);

    initialize(element, parent_ref);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} //namespace adobe
