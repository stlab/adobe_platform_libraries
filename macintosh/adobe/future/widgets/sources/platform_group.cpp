/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_group.hpp>

#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

template <>
inline dictionary_t extra_widget_context(const group_t& w)
{
    static_name_t has_label(w.name_m.empty() ? "false" : "true");
    dictionary_t  result;

    result.insert(std::make_pair(static_name_t("label"), any_regular_t(has_label)));

    return result;
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(group_t, "<xstr id='metric:group'/>");

/****************************************************************************************************/

group_t::group_t(const std::string& name,
                 const std::string& alt_text,
                 theme_t            theme) :
    control_m(0),
    name_m(name),
    alt_text_m(alt_text),
    theme_m(theme)
{ }

/****************************************************************************************************/

void group_t::measure(extents_t& result)
{
    // On the Mac OS, groups don't consider their name when they're
    // doing measurement, so we have to do it for them.

	result = implementation::measure_theme_text(name_m, theme_m);

    implementation::set_metric_extractor(*this);

    result.width() += metrics_m(implementation::k_metric_spacing);

    result.vertical().frame_m.first = result.height();

    result = implementation::apply_fudges(*this, result);
}

/****************************************************************************************************/

void group_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

template <>
platform_display_type insert(display_t& display, platform_display_type& parent, group_t& element)
{
    static const ::Rect bounds_s = { 0, 0, 20, 20 };

    ::HIViewRef parent_ref(parent);

    ADOBE_REQUIRE_STATUS(::CreateGroupBoxControl(::GetControlOwner(parent_ref),
                                                 &bounds_s,
                                                 explicit_cast<auto_cfstring_t>(element.name_m).get(),
                                                 true,
                                                 &element.control_m));

    implementation::set_theme(element.control_m, element.theme_m);

    if (!element.alt_text_m.empty())
        implementation::set_control_alt_text(element.control_m, element.alt_text_m);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
