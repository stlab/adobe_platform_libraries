/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_tokens.hpp>

#include <adobe/future/widgets/headers/factory.hpp>

#include <adobe/static_table.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

/*
    REVISIT (sparent) : Token is starting to look like a "binding" abstraction of some sort.
    
    We're going to add a bind function to the factory token to take care of the complexity
    of binding to a layout_sheet or sheet.
    
    REVISIT (sparent) : This code also indicates problems with the current sheet interfaces:
    
    1. There should be some way to build a single index for both sheets to cover the entire
        scope.
    2. The set() functions may become functions which return setter objects and don't need to
        do the lookup.
*/


/*************************************************************************************************/

size_enum_t enumerate_size(const name_t& size)
{
    typedef static_table<name_t, size_enum_t, 4> name_size_table_t;

    static_name_t key_size_large  = "size_large"_name;
    static_name_t key_size_normal = "size_normal"_name;
    static_name_t key_size_small  = "size_small"_name;
    static_name_t key_size_mini   = "size_mini"_name;
    static bool   init(false);

    static name_size_table_t size_table =
    {{
        name_size_table_t::entry_type(key_size_large,  size_normal_s), // REVISIT (fbrereto) : stubbed to normal
        name_size_table_t::entry_type(key_size_normal, size_normal_s),
        name_size_table_t::entry_type(key_size_small,  size_small_s),
        name_size_table_t::entry_type(key_size_mini,   size_mini_s)
    }};

    if (!init)
    {
        size_table.sort();

        init = true;
    }

    return size_table(size);
}

/*************************************************************************************************/

theme_t size_to_theme(size_enum_t size)
{
    return size == size_small_s ? theme_small_s :
           size == size_mini_s  ? theme_mini_s :
                                  theme_normal_s;
}

/*************************************************************************************************/

touch_set_t touch_set(const dictionary_t& parameters)
{
    array_t  touch_set_array;
    touch_set_t     touch_set;

    get_value(parameters, key_touch, touch_set_array);
    
    /*
    REVISIT (sparent) : interesting - needs to be a way to treat an array as a homogenous type.
    */
    
    // touch_set_m.insert(touch_set_m.end(), touch_set_array.begin(), touch_set_array.end());
    
    for (array_t::const_iterator iter(touch_set_array.begin()),
                last(touch_set_array.end()); iter != last; ++iter)
    {
        touch_set.push_back(iter->cast<name_t>());
    }

    return touch_set;
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/


/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
