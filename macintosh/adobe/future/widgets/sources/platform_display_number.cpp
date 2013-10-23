/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/widgets/headers/platform_display_number.hpp>

#include <adobe/future/number_formatter.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

std::string set_field_text(std::string                       label,
                           double                            value,
                           const std::vector<adobe::unit_t>& unit_set,
                           std::string::size_type*           label_chars=0)
{
    std::stringstream result;

    // this is called by a subview when its value changes.
    // we are only concerned with changing our actual value when
    // the view that changed is also the current view.

    std::string               suffix;
    adobe::number_formatter_t number_formatter;

    if (!unit_set.empty())
    {
        std::vector<adobe::unit_t>::const_iterator i(unit_set.begin());
        std::vector<adobe::unit_t>::const_iterator end(unit_set.end());

        while (i != end && i->scale_m_m <= value)
            ++i;

        if (i != unit_set.begin())
            --i;

        value = adobe::to_base_value(value, *i);

        suffix = i->name_m;

        number_formatter.set_format(i->format_m);
    }

    if (!label.empty())
    {
        result << label << " ";

        if(label_chars)
            *label_chars = label.length();
    }

    result << number_formatter.format(value);

    if (!suffix.empty())
        result << " " << suffix;

    return result.str();
}

/**************************************************************************************************/

void initialize(adobe::display_number_t& control, ::HIViewRef parent)
{
    static const ::Rect bounds_s = { 0, 0, 2048, 2048 };

    ::ControlRef tmp_control;

    ADOBE_REQUIRE_STATUS(::CreateStaticTextControl(::GetControlOwner(parent),
                                                   &bounds_s,
                                                   NULL,
                                                   NULL,
                                                   &tmp_control));

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(tmp_control, control.alt_text_m);

    control.control_m.reset(tmp_control);

    adobe::implementation::set_theme(control, control.theme_m);
}

/**************************************************************************************************/

} // namespace
	
/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(display_number_t, "<xstr id='metric:display_number'/>");

/**************************************************************************************************/

void measure(display_number_t& value, extents_t& result)
{
    assert(value.control_m.get());

    extents_t space_extents(implementation::measure_theme_text(std::string(" "), value.theme_m));
    extents_t unit_max_extents;
    extents_t label_extents(implementation::measure_theme_text(value.name_m, value.theme_m));
    extents_t characters_extents =
        implementation::measure_theme_text(std::string(value.characters_m, '0'), value.theme_m);

    for (display_number_t::unit_set_t::iterator iter(value.unit_set_m.begin()),
         end(value.unit_set_m.end()); iter != end; ++iter)
        {
            extents_t tmp(implementation::measure_theme_text(iter->name_m, value.theme_m));

            if (tmp.width() > unit_max_extents.width())
                unit_max_extents = tmp;
        }

    // set up default settings (baseline, etc.)
    result = space_extents;

    // set the width to the label width (if any)
    result.width() = label_extents.width();

    // add a guide for the label
    result.horizontal().guide_set_m.push_back(label_extents.width());

    // if there's a label, add space for a space
    if (label_extents.width() != 0)
        result.width() += space_extents.width();

    // append the character extents (if any)
    result.width() += characters_extents.width();

    // if there are character extents, add space for a space
    if (characters_extents.width() != 0)
        result.width() += space_extents.width();

    // append the max unit extents (if any)
    result.width() += unit_max_extents.width();

    // adjust and go home
    result = implementation::apply_fudges(value, result);
}

/**************************************************************************************************/

void measure_vertical(display_number_t&   value,
                      extents_t&          result, 
                      const place_data_t& placed_horizontal)
{
    // Note (fbrereto) : This is explicit (instead of using implementation::measure) because
    //                   we need to set the inbound rect to be the potential dimensions of the
    //                   text so the reflow will shrink the bounds if it needs to.

    assert(value.control_m.get());

    ::Rect           old_bounds = { 0 };

    ::GetControlBounds(value.control_m.get(), &old_bounds);

    ::Rect           static_bounds = { 0, 0, 2048, 
                        static_cast<short>(placed_horizontal.horizontal().length_m) };
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

/**************************************************************************************************/

void place(display_number_t& value, const place_data_t& place_data)
{
    implementation::set_bounds(value, place_data);
}

/**************************************************************************************************/

void display(display_number_t& widget, const display_number_t::model_type& value)
{
	std::string base_text = set_field_text(widget.name_m, value, widget.unit_set_m);

    implementation::set_widget_data(widget.control_m.get(),
                                    kControlEntireControl,
                                    kControlStaticTextCFStringTag,
                                    explicit_cast<auto_cfstring_t>(base_text).get());
}

/**************************************************************************************************/

platform_display_type get_display(const display_number_t& widget)
    { return widget.control_m.get(); }

/**************************************************************************************************/

template <>
platform_display_type insert<display_number_t>(display_t&               display,
                                                      platform_display_type&   parent,
                                                      display_number_t& element)
{
    initialize(element, parent);

    return display.insert(parent, get_display(element));
}              

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
