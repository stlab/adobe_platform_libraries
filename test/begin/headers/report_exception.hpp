/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_REPORT_EXCEPTION_HPP
#define ADOBE_REPORT_EXCEPTION_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>
#include "express_viewer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <adobe/istream.hpp>
#include <adobe/adam.hpp>
#include <adobe/name.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

typedef boost::function<void (const std::string& msg)> report_exception_proc_t;

/****************************************************************************************************/
//
/// Formats the exception currently being thrown and sends it off to
/// the callback previously specified by set_report_exception_callback
//

inline void report_exception()
{
    std::string err("Exception: ");

    try { throw; }
    catch (const adobe::stream_error_t& error)
    {
        err += format_stream_error(error);
    }
    catch (const std::exception& error)
    {
        err += error.what();
    }
    catch (...)
    {
        err += "Unknown.";
    }

    application_t::getInstance()->display_error(err);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif // ADOBE_REPORT_EXCEPTION_HPP

/****************************************************************************************************/
