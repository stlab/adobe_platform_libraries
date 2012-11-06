/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/file_monitor.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/functional.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <stdexcept>
#include <utility>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::vector<adobe::file_monitor_platform_data_t*> file_set_t;

/****************************************************************************************************/

inline file_set_t& file_set()
{
    static file_set_t file_set_s;

    return file_set_s;
}

/****************************************************************************************************/

void file_monitor_loop(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/)
{
    file_set_t& set(file_set());

    if (set.empty())
        return;

    for (file_set_t::iterator first(set.begin()), last(set.end()); first != last; ++first)
    {
        if ((*first)->change_handle_m == 0 || (*first)->last_write_m == 0)
            continue;

        DWORD wait_result(WaitForSingleObject((*first)->change_handle_m, 0));

        if (wait_result == WAIT_OBJECT_0)
        {
            std::time_t new_write_time = boost::filesystem::last_write_time((*first)->path_m);

            if (new_write_time != (*first)->last_write_m)
            {
                (*first)->last_write_m = new_write_time;

                (*first)->proc_m((*first)->path_m, adobe::file_monitor_contents_changed_k);
            }
        }
        else if (wait_result == WAIT_FAILED)
        {
            DWORD last_err = 0;

			last_err = ::GetLastError();	// lbourdev: Moved this on a separate line to fix an 'unused variable' warning in gcc 3.4

            throw std::runtime_error("wait error");
        }
    }
}

/****************************************************************************************************/

void install_timer()
{
    static bool inited(false);
    
    if (inited)
        return;

    ::SetTimer(NULL, 0, 100, (TIMERPROC)&file_monitor_loop);

    inited = true;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

file_monitor_platform_data_t::file_monitor_platform_data_t() :
    last_write_m(0),
    change_handle_m(0)
    { file_set().push_back(this); }

file_monitor_platform_data_t::file_monitor_platform_data_t(const file_monitor_path_type&  path,
                                                           const file_monitor_callback_t& proc) :
    path_m(path),
    proc_m(proc),
    last_write_m(0),
    change_handle_m(0)
{
    install_timer();

    connect();
}

file_monitor_platform_data_t::file_monitor_platform_data_t(const file_monitor_platform_data_t& rhs) :
    path_m(rhs.path_m),
    proc_m(rhs.proc_m),
    last_write_m(0),
    change_handle_m(0)
{
    connect();
}

file_monitor_platform_data_t::~file_monitor_platform_data_t()
{
    file_set_t::iterator result(adobe::find(file_set(), this));

    if (result != file_set().end())
        file_set().erase(result); 

    disconnect();
}

file_monitor_platform_data_t& file_monitor_platform_data_t::operator = (const file_monitor_platform_data_t& rhs)
{
    disconnect();

    path_m = rhs.path_m;
    proc_m = rhs.proc_m;

    connect();

    return *this;
}

void file_monitor_platform_data_t::set_path(const file_monitor_path_type& path)
{
    if (!boost::filesystem::exists(path) || path_m == path)
        return;

    install_timer();

    disconnect();

    path_m = path;

    connect();
}

void file_monitor_platform_data_t::connect()
{
    if (!boost::filesystem::exists(path_m))
        return;

    last_write_m = boost::filesystem::last_write_time(path_m);

    std::string change_handle_path(path_m.branch_path().native_directory_string());

    change_handle_m = FindFirstChangeNotificationA(change_handle_path.c_str(),
                                                   FALSE,
                                                   FILE_NOTIFY_CHANGE_FILE_NAME |
                                                   FILE_NOTIFY_CHANGE_SIZE |
                                                   FILE_NOTIFY_CHANGE_LAST_WRITE);

    if (change_handle_m == INVALID_HANDLE_VALUE)
        throw std::runtime_error("bad change handle");
}

void file_monitor_platform_data_t::disconnect()
{
    if (change_handle_m == 0 || last_write_m == 0)
        return;

    FindCloseChangeNotification(change_handle_m);
}

/****************************************************************************************************/

file_monitor_t::file_monitor_t()
    { }

file_monitor_t::file_monitor_t(const file_monitor_path_type& path, const file_monitor_callback_t& proc) :
    plat_m(path, proc)
    { }

void file_monitor_t::set_path(const file_monitor_path_type& path)
    { plat_m.set_path(path); }

void file_monitor_t::monitor(const file_monitor_callback_t& proc)
    { plat_m.proc_m = proc; }

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
