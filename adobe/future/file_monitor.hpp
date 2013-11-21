/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_FILE_MONITOR_HPP
#define ADOBE_FILE_MONITOR_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

enum file_monitor_event_t
{
    file_monitor_contents_changed_k = 0,
    file_monitor_other_k // later expand this to deleted, moved, renamed, etc.
};

typedef boost::filesystem::path file_monitor_path_type;

typedef boost::function<void (const file_monitor_path_type& path, file_monitor_event_t what)> file_monitor_callback_t;

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#include <adobe/future/platform_file_monitor_impl.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

class file_monitor_t
{
public:
    file_monitor_t();

    file_monitor_t(const file_monitor_path_type& path, const file_monitor_callback_t& proc);

    void set_path(const file_monitor_path_type& path);

    void monitor(const file_monitor_callback_t& proc);

private:
    file_monitor_platform_data_t plat_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
