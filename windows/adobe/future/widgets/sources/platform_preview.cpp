/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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

preview_t::preview_t(const std::string& alt_text,
                     theme_t            theme) :
    alt_text_m(alt_text),
    sublayout_m(theme),
    theme_m(theme)
{ }

/****************************************************************************************************/

void preview_t::measure(extents_t& result)
{
    sublayout_m.measure(result);
}

/****************************************************************************************************/

void preview_t::place(const place_data_t& place_data)
{
    sublayout_m.place(place_data);
}

/****************************************************************************************************/

void preview_t::display(const view_model_type& value)
{
    sublayout_m.sublayout_sheet_set_update("image"_name, value);
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
    return parent;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
