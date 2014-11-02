/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_BUTTON_FACTORY_HPP
#define ADOBE_BUTTON_FACTORY_HPP

#include <adobe/dictionary.hpp>

/*
    Hi. My name is Joe, and I work in a but-ton fac-to-ry!
*/

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct widget_node_t;
struct factory_token_t;
class widget_factory_t;

/****************************************************************************************************/

widget_node_t make_button(const dictionary_t&     parameters, 
                          const widget_node_t&    parent, 
                          const factory_token_t&  token,
                          const widget_factory_t& factory);

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
