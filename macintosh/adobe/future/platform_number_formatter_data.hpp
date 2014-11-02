/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_PLATFORM_NUMBER_FORMATTER_DATA_HPP
#define ADOBE_PLATFORM_NUMBER_FORMATTER_DATA_HPP

/****************************************************************************************************/

#include <adobe/any_regular.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>

#include <string>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct number_formatter_platform_data_t
{
    number_formatter_platform_data_t()
    { initialize(); }

    void          initialize();
    void          set_format(const std::string& format);
    std::string   get_format() const;
    std::string   format(const any_regular_t& x);
    any_regular_t parse(const std::string& str, any_regular_t the_type);
    void          monitor_locale(const dictionary_t&);

    auto_resource< ::CFNumberFormatterRef > formatter_m;

    // not for public consumption
    void do_format_update();
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_PLATFORM_NUMBER_FORMATTER_DATA_HPP
#endif

/****************************************************************************************************/
