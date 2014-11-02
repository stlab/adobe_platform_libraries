/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/source/cursor_stack.hpp>

#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

std::vector<adobe_cursor_t>& cursor_stack()
{
    static std::vector<adobe_cursor_t> cursor_stack_s;

    return cursor_stack_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

void cursor_stack_push(adobe_cursor_t cursor)
{
    cursor_stack().push_back(cursor);
}

/****************************************************************************************************/

void cursor_stack_pop()
{
    cursor_stack().pop_back();
}

/****************************************************************************************************/

adobe_cursor_t cursor_stack_top()
{
    return cursor_stack().empty() ? static_cast<adobe_cursor_t>(0) : cursor_stack().back();
}

/****************************************************************************************************/

void cursor_stack_reset()
{
    return cursor_stack().clear();
}

/****************************************************************************************************/
