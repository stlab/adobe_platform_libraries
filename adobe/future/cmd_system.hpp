/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_COMMAND_SYSTEM_HPP
#define ADOBE_COMMAND_SYSTEM_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/name.hpp>

#include <boost/signals2.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#ifndef NDEBUG
    #include <iostream>
#endif

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

class command_system_t : boost::noncopyable
{
public:
    typedef boost::function<void ()>        command_proc_t;
    typedef boost::function<void (bool)>    command_enabled_callback_t;
    typedef boost::signals2::connection     connection_t;

    command_system_t();
    ~command_system_t();

    void insert_command(name_t name, const command_proc_t& proc);
    void remove_command(name_t name);
    void enable_command(name_t name, bool enabled);
    void do_command(name_t name);

    connection_t monitor_enabled(name_t name, const command_enabled_callback_t& callback);

private:
    struct implementation_t;

    implementation_t* object_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
