/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/platform_locale_data.hpp>

#include <adobe/future/locale.hpp>
#include <adobe/future/macintosh_cast.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>
#include <adobe/macintosh_string.hpp>
#include <adobe/name.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef adobe::auto_resource< ::CFLocaleRef > auto_locale_t;

/****************************************************************************************************/

std::string get_locale_tidbit(::CFStringRef property_key)
{
    auto_locale_t          current_locale(::CFLocaleCopyCurrent());
    ::CFTypeRef            value(::CFLocaleGetValue(current_locale.get(), property_key));
    ::CFStringRef          value_string(adobe::cf_cast< ::CFStringRef >(value));

    return value_string ? adobe::explicit_cast<std::string>(value_string) : std::string();
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

void do_locale_check()
{
    static std::string locale_identifier_s;

    auto_locale_t current_locale(::CFLocaleCopyCurrent());
    std::string   new_locale_identifier(get_locale_tidbit(kCFLocaleIdentifier));

    if (locale_identifier_s != new_locale_identifier)
    {
        locale_identifier_s = new_locale_identifier;

        dictionary_t new_locale_data;

        // first collect all the relevant information for the locale dictionary

        std::string decimal_point(get_locale_tidbit(kCFLocaleDecimalSeparator));
        std::string thousands_separator(get_locale_tidbit(kCFLocaleGroupingSeparator));

        // finally push all the relevant information to the locale dictionary

        new_locale_data.insert(std::make_pair(key_locale_identifier, new_locale_identifier));
        new_locale_data.insert(std::make_pair(key_locale_decimal_point, decimal_point));
        new_locale_data.insert(std::make_pair(key_locale_thousands_separator, thousands_separator));

        implementation::signal_locale_change(new_locale_data);
    }
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
