/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_CURSOR_STACK_HPP
#define ADOBE_CURSOR_STACK_HPP

/****************************************************************************************************/

#include <adobe/future/cursor.hpp>

/****************************************************************************************************/

void           cursor_stack_push(adobe_cursor_t cursor);
void           cursor_stack_pop();
adobe_cursor_t cursor_stack_top();
void           cursor_stack_reset();

/****************************************************************************************************/

// ADOBE_CURSOR_STACK_HPP
#endif

/****************************************************************************************************/
