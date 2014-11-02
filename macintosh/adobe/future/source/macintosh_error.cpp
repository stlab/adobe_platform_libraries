/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/macintosh_error.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

inline adobe::error_handler_proc_t& error_handler_proc()
{
    static adobe::error_handler_proc_t error_handler_proc_s;

    return error_handler_proc_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void set_error_handler(const error_handler_proc_t& proc)
{
    error_handler_proc() = proc;
}

/****************************************************************************************************/

void report_error(const std::string& error)
{
    if (error_handler_proc())
        error_handler_proc()(error);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
