/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/platform_number_formatter_data.hpp>

#include <boost/cstdint.hpp>

#include <adobe/future/locale.hpp>
#include <adobe/future/macintosh_error.hpp>
#include <adobe/future/number_formatter.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_string.hpp>

#include <limits>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef adobe::auto_resource< ::CFLocaleRef >          auto_locale_t;
typedef adobe::auto_resource< ::CFNumberFormatterRef > auto_formatter_t;

/****************************************************************************************************/

template <typename Numeric> struct type_to_cfnumbertype;
template <> struct type_to_cfnumbertype<char>       { static const ::CFNumberType value = kCFNumberCharType; };
template <> struct type_to_cfnumbertype<short>      { static const ::CFNumberType value = kCFNumberShortType; };
template <> struct type_to_cfnumbertype<int>        { static const ::CFNumberType value = kCFNumberIntType; };
template <> struct type_to_cfnumbertype<long>       { static const ::CFNumberType value = kCFNumberLongType; };
//template <> struct type_to_cfnumbertype<long long>  { static const ::CFNumberType value = kCFNumberLongLongType; };
template <> struct type_to_cfnumbertype<float>      { static const ::CFNumberType value = kCFNumberFloatType; };
template <> struct type_to_cfnumbertype<double>     { static const ::CFNumberType value = kCFNumberDoubleType; };

//template <> struct type_to_cfnumbertype<unsigned long long> { static const ::CFNumberType value = kCFNumberLongLongType; };

/****************************************************************************************************/

template <typename Numeric>
std::string number_format(::CFNumberFormatterRef formatter, const Numeric& x)
{
    ::CFNumberType         temp_t(type_to_cfnumbertype<Numeric>::value);
    adobe::auto_cfstring_t temp_string(::CFNumberFormatterCreateStringWithValue(NULL,
                                                                                formatter,
                                                                                temp_t,
                                                                                &x));

    return adobe::explicit_cast<std::string>(temp_string.get());
}

/****************************************************************************************************/

template <typename Numeric>
Numeric number_parse(::CFNumberFormatterRef formatter, const std::string& str)
{
    Numeric result(Numeric(0));

    ::CFNumberFormatterGetValueFromString(formatter,
                                          adobe::explicit_cast<adobe::auto_cfstring_t>(str).get(),
                                          NULL,
                                          type_to_cfnumbertype<Numeric>::value,
                                          &result);
    return result;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void number_formatter_platform_data_t::initialize()
{
    monitor_locale(dictionary_t());
}

/****************************************************************************************************/

void number_formatter_platform_data_t::set_format(const std::string& format)
{
    if (formatter_m.get() == 0)
        return;

    ::CFNumberFormatterSetFormat(formatter_m.get(), explicit_cast<auto_cfstring_t>(format).get());
}

/****************************************************************************************************/

std::string number_formatter_platform_data_t::get_format() const
{
    if (formatter_m.get() == 0)
        return std::string();

    return explicit_cast<std::string>(::CFNumberFormatterGetFormat(formatter_m.get()));
}

/****************************************************************************************************/

std::string number_formatter_platform_data_t::format(const any_regular_t& x)
{
    assert(formatter_m);

    if (x.type_info() == type_info<double>()) {
        return number_format<double>(formatter_m.get(), x.cast<double>());
    }
    
#if 0
    // REVISIT (sparent) : Pulled 64 bit support for formatting - do we need this?
    //else if (x.type() == typeid(boost::intmax_t)) return number_format<boost::intmax_t>(formatter_m.get(), x.cast<boost::intmax_t>());
    //else if (x.type() == typeid(boost::uintmax_t)) return number_format<boost::uintmax_t>(formatter_m.get(), x.cast<boost::uintmax_t>());
#endif

    else return std::string("formatter_format_number error");
}

/****************************************************************************************************/

any_regular_t number_formatter_platform_data_t::parse(const std::string& str, any_regular_t the_type)
{
    assert(formatter_m);

    if (the_type.type_info() == type_info<double>()) {
        return any_regular_t(number_parse<double>(formatter_m.get(), str));
    }
    
#if 0
    // REVISIT (sparent) : Pulled 64 bit support for formatting - do we need this?
    //else if (the_type.type() == typeid(boost::intmax_t))  return any_regular_t(number_parse<boost::intmax_t>(formatter_m.get(), str));
    //else if (the_type.type() == typeid(boost::uintmax_t))  return any_regular_t(number_parse<boost::uintmax_t>(formatter_m.get(), str));
#endif
    else return any_regular_t(std::string("formatter_format_number error"));
}

/****************************************************************************************************/

void number_formatter_platform_data_t::monitor_locale(const dictionary_t&)
{
    auto_locale_t current_locale(::CFLocaleCopyCurrent());
    std::string   num_format_save(get_format());

    formatter_m.reset(::CFNumberFormatterCreate(NULL, current_locale.get(), kCFNumberFormatterNoStyle));

    set_format(num_format_save);
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

bool completely_valid_number_string_given_current_locale(const std::string& value)
{
    auto_locale_t    current_locale(::CFLocaleCopyCurrent());
    auto_formatter_t formatter(::CFNumberFormatterCreate(NULL, current_locale.get(),
                                                         kCFNumberFormatterNoStyle));
    ::CFRange        range = { 0 };
    double           result(std::numeric_limits<double>::min());

    range.length = value.size();

    //::Boolean success =
        ::CFNumberFormatterGetValueFromString(formatter.get(),
                                              explicit_cast<auto_cfstring_t>(value).get(),
                                              &range,
                                              kCFNumberDoubleType,
                                              &result);

    return static_cast<double>(range.length) == static_cast<double>(value.size());
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

void number_formatter_t::set_format(const std::string& format)
{ data_m.set_format(format); }

/****************************************************************************************************/

std::string number_formatter_t::get_format() const
{ return data_m.get_format(); }

/****************************************************************************************************/

template <>
std::string number_formatter_t::format<any_regular_t>(const any_regular_t& x)
{ return data_m.format(x); }

/****************************************************************************************************/

template <>
any_regular_t number_formatter_t::parse<any_regular_t>(const std::string& str, any_regular_t dummy)
{ return data_m.parse(str, dummy); }

/****************************************************************************************************/

void number_formatter_t::monitor_locale(const dictionary_t& locale_data)
{ return data_m.monitor_locale(locale_data); }

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
