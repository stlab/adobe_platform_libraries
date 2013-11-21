/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_NUMBER_FORMATTER_IMPL_HPP
#define ADOBE_NUMBER_FORMATTER_IMPL_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/dictionary.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct number_formatter_platform_data_t
{
    number_formatter_platform_data_t()
    { initialize(); }

    void initialize()
    { }
    void set_format(const std::string& format)
    { format_m = format; }
    std::string get_format() const
    { return format_m; }
    std::string format(const any_regular_t& x)
    { return std::string(); }
    any_regular_t parse(const std::string& str, any_regular_t the_type)
    { return any_regular_t(); }
    void monitor_locale(const dictionary_t& locale_data)
    { }

    std::string format_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_NUMBER_FORMATTER_IMPL_HPP
#endif

/****************************************************************************************************/
