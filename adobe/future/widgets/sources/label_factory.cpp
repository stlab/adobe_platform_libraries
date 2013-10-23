/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_label.hpp>

#include <adobe/future/widgets/headers/factory.hpp>
#include <adobe/future/widgets/headers/label_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory_registry.hpp>
#include <adobe/future/widgets/headers/widget_tokens.hpp>

#include <adobe/static_table.hpp>


/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void create_widget(const dictionary_t& parameters,
                   size_enum_t         size,
                   label_t*&           widget)
{
    std::string name;
    std::string alt_text;
    long        characters(0);

    get_value(parameters, key_name, name);
    get_value(parameters, key_alt_text, alt_text);
    get_value(parameters, key_characters, characters);

    widget = new label_t(name, alt_text, characters, 
                         implementation::size_to_theme(size));
}

/****************************************************************************************************/

template <>
void attach_view_and_controller(label_t&,
                                const dictionary_t&,
                                const factory_token_t&,
                                adobe::name_t,
                                adobe::name_t,
                                adobe::name_t)
{
    // no adam interaction
}

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

widget_node_t make_label_hack(const dictionary_t&     parameters,
                              const widget_node_t&    parent,
                              const factory_token_t&  token,
                              const widget_factory_t& factory)
{ 
    return create_and_hookup_widget<label_t, poly_placeable_twopass_t>(
        parameters, parent, token, 
        factory.is_container(static_name_t("label")), 
        factory.layout_attributes(static_name_t("label"))); 
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

