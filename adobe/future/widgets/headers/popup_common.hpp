/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_POPUP_COMMON_HPP
#define ADOBE_POPUP_COMMON_HPP

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_popup.hpp>

#include <adobe/array.hpp>
#include <adobe/name.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/future/assemblage.hpp>
#include <adobe/future/widgets/headers/factory.hpp>
#include <adobe/future/widgets/headers/widget_tokens.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

static popup_t::menu_item_set_t array_to_menu_item_set(const array_t& value)
{
    popup_t::menu_item_set_t set;

    for (array_t::const_iterator iter(value.begin()), last(value.end());
         iter != last; ++iter)
    {
        if (iter->type_info() != type_info<dictionary_t>())
            continue;

        const dictionary_t&          cur_new_item(iter->cast<dictionary_t>());
        dictionary_t::const_iterator name_iter(cur_new_item.find(key_name));
        dictionary_t::const_iterator value_iter(cur_new_item.find(key_value));

        if (name_iter == cur_new_item.end() ||
            name_iter->second.type_info() != type_info<string_t>() ||
            value_iter == cur_new_item.end())
            continue;

        set.push_back(popup_t::menu_item_t(name_iter->second.cast<std::string>(), value_iter->second));
    }

    return set;
} 

/****************************************************************************************************/

struct dynamic_menu_item_set_view_t
{
    typedef array_t model_type;

    explicit dynamic_menu_item_set_view_t(popup_t& popup) :
        popup_m(popup)
    { }

    void display(const model_type& value)
    { popup_m.reset_menu_item_set(array_to_menu_item_set(value)); }

    popup_t& popup_m;
};

/****************************************************************************************************/

template <typename Sheet>
void attach_popup_menu_item_set(popup_t&           control,
                                name_t             cell,
                                Sheet&             sheet,
                                assemblage_t&      assemblage,
                                eve_client_holder& /*client_holder*/)
{
    dynamic_menu_item_set_view_t* tmp = new dynamic_menu_item_set_view_t(control);

    assemblage.cleanup(boost::bind(delete_ptr(), tmp));

    attach_view(assemblage, cell, *tmp, sheet);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_POPUP_COMMON_HPP
#endif

/****************************************************************************************************/
