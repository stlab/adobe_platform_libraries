/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_LINK_FACTORY_HPP
#define ADOBE_LINK_FACTORY_HPP

#include <adobe/dictionary.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct widget_node_t;
struct factory_token_t;
class widget_factory_t;

/****************************************************************************************************/

widget_node_t make_link(const dictionary_t&     parameters,
                        const widget_node_t&    parent,
                        const factory_token_t&  token,
                        const widget_factory_t& factory);

/****************************************************************************************************/

inline const layout_attributes_t& link_layout_attributes()
{
    static layout_attributes_t result;
    static bool                       inited(false);

    if (!inited)
    {
        inited = true;

        result.vertical().alignment_m = layout_attributes_t::align_fill;
    }

    return result;
}


/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
