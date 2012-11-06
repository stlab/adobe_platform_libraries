/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

// link.hpp needs to come before widget_factory to hook the overrides
#include <adobe/future/widgets/headers/platform_link.hpp>

#include <adobe/future/widgets/headers/link_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory_registry.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void create_widget(const dictionary_t& parameters,
                   size_enum_t         size,
                   link_t*&            widget)
{
    std::string          alt_text;
    any_regular_t value_on(true);
    any_regular_t value_off(false);
    long                 count(0);

    get_value(parameters, key_alt_text, alt_text);
    get_value(parameters, key_value_on, value_on);
    get_value(parameters, key_value_off, value_off);
    get_value(parameters, key_count, count);

    widget = new link_t(alt_text, value_on, value_off, count, 
        implementation::size_to_theme(size));
}
    
/****************************************************************************************************/

widget_node_t make_link(const dictionary_t&     parameters,
                        const widget_node_t&    parent,
                        const factory_token_t&  token,
                        const widget_factory_t& factory)
{ 
    return create_and_hookup_widget<link_t, poly_placeable_t>(parameters, parent, token, 
        factory.is_container(static_name_t("link")), 
        factory.layout_attributes(static_name_t("link"))); 
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
