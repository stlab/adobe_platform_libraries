/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_BEGIN_FILE_HPP
#define ADOBE_BEGIN_FILE_HPP

#include <adobe/config.hpp>

#include <adobe/enum_ops.hpp>
#include <adobe/file_slurp.hpp>
#include <adobe/future/file_monitor.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/function.hpp>

#include <string>
#include <iostream>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

class file_buffer_t
{
public:
    enum line_ending_t
    {
        line_ending_unknown_k           = 0,
        line_ending_platform_k          = 1 << 0L,
        line_ending_unix_k              = 1 << 1L,           // LF
        line_ending_windows_k           = 1 << 2L,           // CR+LF
        line_ending_mac_os_classic_k    = 1 << 3L,           // CR
        line_ending_mac_os_x_k          = line_ending_unix_k // LF
    };

    typedef boost::function<void (bool)> dirty_proc_t;
    typedef boost::function<void ()>     disk_update_proc_t;

    file_buffer_t();

    void set_path(const bfs::path& path);

    template <typename I>
    void set_contents(I first, I last)
    {
        contents_m.assign(first, last);

        set_dirty(true);
    }

    inline void set_line_endings(line_ending_t le, bool force = false);

    void save();

    bfs::path        directory_path() const
        { return path_m.branch_path(); }

    const bfs::path& get_path() const
        { return path_m; }

    const char*      file_name() const
        { return path_m.string().c_str(); }

    bool             is_valid() const
        { return bfs::exists(path_m); }

    const std::string& as_string() const
        { return contents_m; }

    void               monitor_dirty(dirty_proc_t proc)
        { dirty_proc_m = proc; }

    void               monitor_disk_update(const disk_update_proc_t& proc)
        { disk_update_proc_m = proc; }

private:

    void reload_from_disk();

    void set_dirty(bool to);

    void replace_all(const char* src, const char* dst);

    void set_line_endings_impl(line_ending_t le, bool force = false);

    bool ask_for_path();

    void file_on_disk_event(const boost::filesystem::path& path, file_monitor_event_t what);

    bfs::path          path_m;             ///< Path to the current file
    std::string        contents_m;         ///< Contents of the file (not necessarily same as that in file)
    bool               dirty_m;            ///< Dirty bit
    dirty_proc_t       dirty_proc_m;       ///< Dirty bit modification notifier
    disk_update_proc_t disk_update_proc_m; ///< On-disk updates notifier
    line_ending_t      le_m;               ///< Line ending flags
    file_monitor_t     monitor_m;          ///< monitors on-disk file changes
};

/****************************************************************************************************/

ADOBE_DEFINE_BITSET_OPS(file_buffer_t::line_ending_t)

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
