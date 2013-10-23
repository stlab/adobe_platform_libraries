/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_panel.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

template <>
void shed_fudges<panel_t>(panel_t& control, place_data_t& place_data)
{
    /*
        NOTE (fbrereto) :   Panel overrides shed_fudges because it does not have metrics
                            with which it would otherwise adjust its position and size.
                            Also in the case of panels (it being different from other
                            widgets), we must move the panel closer to 0,0 to accomodate
                            the new outsets. We also must grow the size of the panel to the
                            sum of the outsets for each orientation. This assures us that
                            the panel is able to contain all the children outsets and be
                            in the right place relative to its parent.
    */

    left(place_data) -= place_data.horizontal().outset_m.first;
    top(place_data)  -= place_data.vertical().outset_m.first;

    ::ADOBE_REQUIRE_STATUS(::HIViewSetBoundsOrigin(
        control.control_m,
        -place_data.horizontal().outset_m.first,
        -place_data.vertical().outset_m.first));

    width(place_data)  += place_data.horizontal().outset_m.first +
                          place_data.horizontal().outset_m.second;

    height(place_data) += place_data.vertical().outset_m.first +
                          place_data.vertical().outset_m.second;
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(panel_t, "<xstr id='metric:panel'/>");

/****************************************************************************************************/

panel_t::panel_t(const any_regular_t& show_value,
                 theme_t                     theme) :
    control_m(0),
    show_value_m(show_value),
    theme_m(theme)
{ }

/****************************************************************************************************/

void panel_t::measure(extents_t& result)
{
    result = extents_t(); 
}

/****************************************************************************************************/

void panel_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void panel_t::display(const any_regular_t& value)
{
    set_visible(value == show_value_m);
}

/****************************************************************************************************/

void panel_t::set_visible(bool visible)
{
    implementation::set_visible(control_m, visible);
}

/****************************************************************************************************/

template <>
platform_display_type insert<panel_t>(display_t&             display,
                                             platform_display_type& parent,
                                             panel_t&        element)
{
    static const ::Rect bounds_s = { 0, 0, 10, 10 };

    assert (!element.control_m);

    ::HIViewRef parent_ref(parent);

    ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(::GetControlOwner(parent_ref),
                                                 &bounds_s,
                                                 kControlSupportsEmbedding,
                                                 &element.control_m));

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
