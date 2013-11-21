/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#include <adobe/future/widgets/headers/widget_factory_registry.hpp>

#include <adobe/string.hpp>

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

widget_factory_t::map_type::iterator widget_factory_t::find(name_t name, noconst) const
{
    map_type::iterator result(const_cast<map_type&>(map_m).find(name));

    if (result == map_m.end())
    {
        throw std::runtime_error(make_string("Widget factory method for '", name.c_str(), "'not found."));
    }

    return result;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
