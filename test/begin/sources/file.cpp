/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include "file.hpp"

#include <adobe/future/widgets/headers/alert.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/name.hpp>

#include <boost/bind.hpp>

#include <stdexcept>
#include <vector>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

file_buffer_t::file_buffer_t() :
    le_m(line_ending_unknown_k)
{
    monitor_m = file_monitor_t(boost::filesystem::path(),
                               boost::bind(&file_buffer_t::file_on_disk_event, boost::ref(*this), _1, _2));
}

/****************************************************************************************************/

void file_buffer_t::file_on_disk_event(const boost::filesystem::path& path, file_monitor_event_t what)
{
    if (path_m != path)
        throw std::logic_error("monitored path does not match file buffer path");

    if (!dirty_m)
    {
        reload_from_disk();

        if (disk_update_proc_m)
            disk_update_proc_m();

        return;
    }

    adobe::alert_helper_t helper;
    const char*           confirm("Replace");

    helper.window_name_m = "File On Disk Notification";

    switch (what)
    {
        case adobe::file_monitor_contents_changed_k:
        {
            helper.button_0_m = confirm;
            helper.button_1_m = "Do Not Replace";

            helper.message_m = "The file on disk has changed. Would you like to "
                               "refresh the file contents with the on-disk contents? "
                               "This will eliminate any in-memory changes you have "
                               "made to the file since it was last saved.";
        }
        break;

        case adobe::file_monitor_other_k:
        {
            helper.button_0_m = "OK";

            helper.message_m = "Something has happened to the file on disk. Please "
                               "invesigate.";
        }
        break;
    }

    const char* result = helper.run().first;

    if (result == confirm)
    {
        reload_from_disk();

        if (disk_update_proc_m)
            disk_update_proc_m();
    }
}

/****************************************************************************************************/

void file_buffer_t::reload_from_disk()
{
    adobe::file_slurp<char> slurp(path_m);

    contents_m.assign(slurp.begin(), slurp.end());

    set_line_endings_impl(le_m, true);

    set_dirty(false);
}

/****************************************************************************************************/

void file_buffer_t::set_path(const boost::filesystem::path& path)
{
    path_m = path;

    reload_from_disk();

    monitor_m.set_path(path_m);
}

/****************************************************************************************************/

void file_buffer_t::set_line_endings(line_ending_t le, bool force)
{
    set_line_endings_impl(le, force);

    set_dirty(true);
}

/****************************************************************************************************/

void file_buffer_t::save()
{
    if (!dirty_m)
        return;

    if (!is_valid() && !ask_for_path())
        return;

    bfs::ofstream output( path_m, std::ios_base::out | std::ios_base::binary );

    //
    // Verify that we were able to open the file for writing.
    //

    if ( output.fail() )
        throw std::runtime_error( "Error opening file for writing: \""
            + path_m.string() + "\"" );

    if (le_m == line_ending_unknown_k)
        set_line_endings(le_m);

    output << contents_m;

    set_dirty(false);

    //
    // in the case when the file is new, we need to monitor the file here
    //

    monitor_m.set_path(path_m);
}

/****************************************************************************************************/

bool file_buffer_t::ask_for_path()
{
    adobe::alert_helper_t helper;

    const char* ok_button("OK");
    const char* cancel_button("Cancel");

    helper.window_name_m = "Where Should Adobe Begin Save the File?";
    helper.message_m = "A location has not been specified for where "
                       "you would like to save the file. Would you "
                       "like to select a location?";
    helper.button_0_m = ok_button;
    helper.button_1_m = cancel_button;

    const char* result = helper.run().first;

    if (result != ok_button)
        return false;

#if ADOBE_PLATFORM_MAC || ADOBE_PLATFORM_WIN
    // Pick a file path dialog

    return implementation::pick_save_path(path_m);
#endif

    return false;
}

/****************************************************************************************************/

void file_buffer_t::set_dirty(bool to)
{
    if (dirty_m == to) return;

    dirty_m = to;

    if (dirty_proc_m)
        dirty_proc_m(dirty_m);
}

/****************************************************************************************************/

void file_buffer_t::replace_all(const char* src, const char* dst)
{
    // replaces all instances of src with dst

    std::string::size_type result(0);
    std::size_t            src_n(std::strlen(src));
    std::size_t            dst_n(std::strlen(dst));

    while (true)
    {
        result = contents_m.find(src, result);

        if (result == std::string::npos) break;

        contents_m.replace(result, src_n, dst, dst_n);

        result += dst_n;
    } 
}

/****************************************************************************************************/

void file_buffer_t::set_line_endings_impl(line_ending_t le, bool force)
{
    if (le_m == le && !force && le_m != line_ending_unknown_k)
        return;

    if (le == line_ending_platform_k || le == line_ending_unknown_k)
    {
#if ADOBE_PLATFORM_WIN
        le_m = line_ending_windows_k;
#else
        le_m = line_ending_unix_k;
#endif
    }
    else
    {
        le_m = le;
    }

    // REVISIT (fbrereto) : Not as optimal as it could be.

    if (le_m == line_ending_unix_k)
    {
        replace_all("\r\n", "\n");
        replace_all("\r",   "\n");
    }
    else if (le_m == line_ending_windows_k)
    {
        replace_all("\r\n", "\n");
        replace_all("\r",   "\n");
        replace_all("\n",   "\r\n");
    }
    else if (le_m == line_ending_mac_os_classic_k)
    {
        replace_all("\r\n", "\r");
        replace_all("\n",   "\r");
    }
    else
        throw std::runtime_error("unknown line ending type");

    // set line endings back to 'unknown' if
    // that's what they were originally

    if (le == line_ending_unknown_k)
        le_m = line_ending_unknown_k;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
