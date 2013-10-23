/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/file_monitor.hpp>

#include <adobe/functional.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem/convenience.hpp>

#include <list>
#include <vector>
#include <stdexcept>
#include <utility>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

int file_monitor_kqueue()
{
    static const int kq(kqueue());

    if (kq < 0)
        throw std::runtime_error("bad kqueue");

    return kq;
}

/****************************************************************************************************/

void file_monitor_loop(::EventLoopTimerRef /*inTimer*/, void* /*data*/)
try
{
    static const timespec             timeout = { 0, 0 };
    static std::vector<struct kevent> ev_receive(16);

    while (true)
    {
        int ev_count(kevent(file_monitor_kqueue(), NULL, 0, &ev_receive[0], ev_receive.size(), &timeout));

        if (ev_count <= 0)
            break;

        for (int i(0); i < ev_count; ++i)
        {
            struct kevent* kevp(&ev_receive[i]);

            if (kevp->flags == EV_ERROR )
                throw std::runtime_error("file_monitor_loop err");

            adobe::file_monitor_platform_data_t& file(*reinterpret_cast<adobe::file_monitor_platform_data_t*>(kevp->udata));

            // we have an issue here (at least on Mac OS X) where we get to this
            // point during a notify when then on-disk contents might not have
            // been updated yet. We need to find a good way to know when the
            // source has been updated on disk. Note that size may be updated to
            // 0, so we cannot simply check for when size is nonzero. As a
            // temporary fix we delay for a brief amount so the rest of the file
            // can catch up to the modification time.

            struct timespec brief_pause;
            brief_pause.tv_sec = 1;
            brief_pause.tv_nsec = 0;

            nanosleep(&brief_pause, NULL);

            std::time_t new_write_time = boost::filesystem::last_write_time(file.path_m);

            if (new_write_time != file.last_write_m)
            {
                file.last_write_m = new_write_time;

                file.proc_m(file.path_m, adobe::file_monitor_contents_changed_k);
            }
        }
    }
}
catch (...)
{
    // do something more reasonable here
    ::AudioServicesPlayAlertSound(kUserPreferredAlert);
}

/****************************************************************************************************/

void install_timer()
{
    static bool inited(false);

    if (inited)
        return;

    static adobe::auto_resource< ::EventLoopTimerUPP > loop_upp(::NewEventLoopTimerUPP(file_monitor_loop));
    static adobe::auto_resource< ::EventLoopTimerRef > idle_timer_ref;
    ::EventLoopTimerRef                                temp_timer_ref;

    if (::InstallEventLoopTimer(::GetMainEventLoop(),
                                .01,
                                .01,
                                loop_upp.get(),
                                0,
                                &temp_timer_ref) != noErr)
        throw std::runtime_error("InstallEventLoopTimer");

    idle_timer_ref.reset(temp_timer_ref);

    inited = true;
}

/****************************************************************************************************/

inline struct kevent empty_kevent()
{
    static const struct kevent empty_event_s = { 0 };

    return empty_event_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

file_monitor_platform_data_t::file_monitor_platform_data_t() :
    last_write_m(0),
    fd_m(0),
    evt_m(empty_kevent())
    { }

file_monitor_platform_data_t::file_monitor_platform_data_t(const file_monitor_path_type&  path,
                                                           const file_monitor_callback_t& proc) :
    path_m(path),
    proc_m(proc),
    last_write_m(0),
    fd_m(0),
    evt_m(empty_kevent())
{
    install_timer();

    connect();
}

file_monitor_platform_data_t::file_monitor_platform_data_t(const file_monitor_platform_data_t& rhs) :
    path_m(rhs.path_m),
    proc_m(rhs.proc_m),
    last_write_m(0),
    fd_m(0),
    evt_m(empty_kevent())
{
    connect();
}

file_monitor_platform_data_t::~file_monitor_platform_data_t()
{
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

    static const timespec timeout = { 0, 0 };

    last_write_m = boost::filesystem::last_write_time(path_m);

    // WARNING (fbrereto) : O_EVTONLY might be HFS+ only. (if so use FREAD)
    fd_m = open(path_m.string().c_str(), O_EVTONLY);

    EV_SET(&evt_m,
           fd_m,
           EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_CLEAR,
           NOTE_DELETE | NOTE_WRITE | NOTE_ATTRIB | NOTE_RENAME,
           0,
           this);

    if (kevent(file_monitor_kqueue(), &evt_m, 1, NULL, 0, &timeout) == -1)
        throw std::runtime_error("file monitor connect failed");
}

void file_monitor_platform_data_t::disconnect()
{
    if (fd_m == 0 || last_write_m == 0)
        return;

    static const timespec timeout = { 0, 0 };

    evt_m.fflags = EV_DELETE;

    if (kevent(file_monitor_kqueue(), &evt_m, 1, NULL, 0, &timeout) == -1)
        throw std::runtime_error("file monitor disconnect failed");

    (void)close(fd_m);
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
