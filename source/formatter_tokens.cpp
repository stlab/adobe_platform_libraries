/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/******************************************************************************/

#include <adobe/config.hpp>

#include <adobe/formatter_tokens.hpp>

/******************************************************************************/

namespace adobe {

/******************************************************************************/

#define ADOBE_TOKEN_DEF(x) extern const static_name_t key_##x = { #x };

ADOBE_TOKEN_DEF(cell_type)
ADOBE_TOKEN_DEF(comment_brief)
ADOBE_TOKEN_DEF(comment_detailed)
ADOBE_TOKEN_DEF(conditional)
ADOBE_TOKEN_DEF(expression)
ADOBE_TOKEN_DEF(initializer)
ADOBE_TOKEN_DEF(linked)
// ADOBE_TOKEN_DEF(name) // defined in widget_tokens
ADOBE_TOKEN_DEF(name_set)
ADOBE_TOKEN_DEF(parameters)
ADOBE_TOKEN_DEF(relation_set)

ADOBE_TOKEN_DEF(cell_meta_type)
ADOBE_TOKEN_DEF(meta_type_cell)
ADOBE_TOKEN_DEF(meta_type_relation)
ADOBE_TOKEN_DEF(meta_type_interface)

#undef ADOBE_TOKEN_DEF

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
