/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
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
