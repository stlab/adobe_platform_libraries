/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_preview.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {} // namespace implementation

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(preview_t, "<xstr id='metric:preview'/>");

/****************************************************************************************************/

preview_t::preview_t(const std::string& alt_text,
                     theme_t            theme) :
    control_m(0),
    alt_text_m(alt_text),
    sublayout_m(theme),
    theme_m(theme)
{ }

/****************************************************************************************************/

void preview_t::measure(extents_t& result)
{
    result = extents_t(); 
}

/****************************************************************************************************/

void preview_t::place(const place_data_t& place_data)
{
    assert(control_m);

    implementation::set_bounds(*this, place_data);
}

/****************************************************************************************************/

void preview_t::display(const view_model_type& /*value*/)
{
}

/****************************************************************************************************/

void preview_t::enable(bool /*make_enabled*/)
{
}

/****************************************************************************************************/

void preview_t::monitor(const setter_proc_type& /*proc*/)
{
}

/****************************************************************************************************/

template <>
platform_display_type insert<preview_t>(display_t&             display,
                                        platform_display_type& parent,
                                        preview_t&             element)
{
    static const ::Rect bounds_s = { 0, 0, 10, 10 };

    assert (!element.control_m);

    ::HIViewRef parent_ref(parent);
    ::HIViewRef control(0);

    ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(::GetControlOwner(parent_ref),
                                                 &bounds_s,
                                                 kControlSupportsEmbedding,
                                                 &control));

    element.control_m.reset(control);

    return display.insert(parent, control);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
