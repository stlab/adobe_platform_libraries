/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

// tab_group.hpp needs to come before widget_factory to hook the overrides
#include <adobe/future/widgets/headers/platform_tab_group.hpp>

#include <adobe/future/widgets/headers/tab_group_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory_registry.hpp>
#include <adobe/dictionary.hpp>

/*************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void create_widget(const dictionary_t& parameters,
                   size_enum_t         size,
                   tab_group_t*&       widget)
{
    array_t items;
    
    get_value(parameters, key_items, items);

    std::vector<tab_group_t::tab_t>  tabs;
    array_t::const_iterator          first(items.begin());
    array_t::const_iterator          last(items.end());

    for (; first != last; ++first)
    {
        tab_group_t::tab_t new_tab;

        new_tab.name_m = get_value((*first).cast<dictionary_t>(), key_name).cast<std::string>();
        new_tab.value_m = get_value((*first).cast<dictionary_t>(), key_value);

        tabs.push_back(new_tab);
    } 

    tab_group_t::tab_t* first_tab(tabs.empty() ? 0 : &tabs[0]);
    theme_t theme(implementation::size_to_theme(size));

    widget = new tab_group_t(first_tab, first_tab + tabs.size(), theme);
}

/****************************************************************************************************/

widget_node_t make_tab_group(const dictionary_t&     parameters,
                             const widget_node_t&    parent,
                             const factory_token_t&  token,
                             const widget_factory_t& factory)
{ 
    return create_and_hookup_widget<tab_group_t, poly_placeable_t>(
        parameters, parent, token, 
        factory.is_container("tab_group"_name), 
        factory.layout_attributes("tab_group"_name)); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
