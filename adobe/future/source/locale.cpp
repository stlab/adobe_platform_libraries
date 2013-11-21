/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/future/locale.hpp>

#include <adobe/future/platform_locale_data.hpp>

#include <adobe/future/periodical.hpp>
#include <adobe/name.hpp>
#include <adobe/once.hpp>

/**************************************************************************************************/

using namespace std;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

typedef boost::signals2::signal<void (const adobe::dictionary_t&)> locale_signal_type;

/**************************************************************************************************/

void init_locale_once()
{
    static adobe::periodical_t periodical(&adobe::implementation::do_locale_check, 1000);

    // get the current locale information as part of the initialization
    adobe::implementation::do_locale_check();
}

/**************************************************************************************************/

void locale_once()
{
    static once_flag flag;
    call_once(flag, &init_locale_once);
}

/**************************************************************************************************/

adobe::dictionary_t& current_locale_data()
{
    static adobe::dictionary_t locale_s;

    return locale_s;
}

locale_signal_type& locale_signal()
{
    static locale_signal_type signal_s;

    return signal_s;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

boost::signals2::connection monitor_locale(const monitor_locale_proc_t& proc)
{
    locale_once();

    // signal the client right off the bat with the current locale information
    proc(current_locale_data());

    return locale_signal().connect(proc, boost::signals2::at_back);
}

const dictionary_t& current_locale()
{
    locale_once();

    return current_locale_data();
}

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

void signal_locale_change(const dictionary_t& new_locale_data)
{
    current_locale_data() = new_locale_data;

    (locale_signal())(new_locale_data);
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

static_name_t key_locale_identifier          = "key_locale_identifier"_name;
static_name_t key_locale_decimal_point       = "key_locale_decimal_point"_name;
static_name_t key_locale_thousands_separator = "key_locale_thousands_separator"_name;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
