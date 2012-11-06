/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_EXPRESS_VIEWER_HPP
#define ADOBE_EXPRESS_VIEWER_HPP

#include <adobe/config.hpp>

#include <string>
#include <iostream>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <adobe/adam.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/file_slurp.hpp>
#include <adobe/future/widgets/headers/virtual_machine_extension.hpp>
#include <adobe/future/widgets/headers/window_server.hpp>

#include "file.hpp"

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

//
/// The stream_sucker is used to extract the data available from a stream and
/// deposit it into a string. This is not very efficient but works for a
/// demo application.
///
/// \param  stream  the stream to extract data from.
/// \return all of the data in the stream.
//

/*
    REVISIT (sparent) : The lf handling should be moved down into where ever this data is going -
    In this case into a UI widget. The general rule should everything should be agnostic about
    line endings on read. And everything should write Unix style (just '\n') line endings.
*/

template<typename InputStream>
std::string stream_sucker( InputStream& stream )
{
#if ADOBE_PLATFORM_MAC
    const char platform_lf('\r');
#else
    const char platform_lf('\n'); // Windows seems to handle this fine
#endif

    std::string result;

    if ( stream.is_open() )
    {
        stream.unsetf(std::ios_base::skipws);
        
        std::istream_iterator<char> first(stream);
        std::istream_iterator<char> last;
        
        while (first != last) {
            if (is_line_end(first, last))
            {
                result.append(1, platform_lf);
                continue;
            }
            result.append(1, *first);
            ++first;
        }
    }

    return result;
}

/****************************************************************************************************/

//
/// The application_t class is implemented in express_viewer.cpp and defines
/// the basic application operation. The application_t class is a singleton,
/// so you must use application_t::getInstance() to get an instance.
//
class application_t
{
    file_buffer_t                             _eve_file_m;        ///< The file buffer for manipulating the current Eve definition.
    file_buffer_t                             _adam_file_m;       ///< The file buffer for manipulating the current Adam definition.
    adobe::sheet_t*                           _sheet_m;           ///< The sheet we are viewing.
    adobe::behavior_t                         _behavior_m;        ///< Root relayout behavior
    adobe::vm_lookup_t                        _vm_lookup_m;       ///< The vm lookup function object for custom proc specifications.
    adobe::window_server_t*                   _holder_m;          ///< Contains Eve instance, and all instanciated widgets.
    adobe::sheet_t                            _editor_sheet_m;    ///< The sheet for the editor window.
    adobe::behavior_t                         _editor_behavior_m; ///< Root relayout behavior for the editor palette.
    adobe::auto_ptr<adobe::eve_client_holder> _editor_holder_m;   ///< The client_holder for the editor window.
    size_enum_t                               _dialog_size_m;     ///< The size of dialog to create.
    bool                                      _initialized;       ///< Are we initialized yet?

    //
    /// This constructor is private to ensure that the getInstance
    /// method is used to return a reference to this class.
    //
    application_t();
    //
    /// This destructor releases any allocated sheets and Eve bits.
    //
    ~application_t();
    //
    /// Initialize the application_t by attempting to load resources from
    /// disk. This also calls os_initialize, to perform any OS-specific
    /// initialization which may be required.
    ///
    /// \return true if the initialization is successful, false otherwise.
    /// \sa     os_initialize
    //
    bool _initialize();
    //
    /// This function gets invoked whenever an action (e.g.: a button with
    /// "action: @ok" defined) is unhandled by Adam. It is registered with
    /// _holder_m by initialize(). Currently this function simply writes the
    /// given value into the output text area.
    ///
    /// \param  name    the name of the action (e.g.: "ok", "cancel").
    /// \param  value   any parameter associated with the value.
    ///
    /// \sa     _holder_m
    //
    void _button_notifier( const adobe::name_t& name, const adobe::any_regular_t& value );
    //
    /// This function handles most editor operations, as defined in the 
    /// editor's Eve file.
    ///
    /// \param  name    the name of the operation to perform.
    /// \param  value   a dictionary containing the value specific to the operation.
    //
    void _editor_op( adobe::name_t name, const adobe::any_regular_t& value );
    //
    /// Called by _editor_op; a function for a subset of the editor window actions
    ///
    /// \param  name    the name of the operation to perform.
    /// \param  value   a dictionary containing the value specific to the operation.
    //
    void _sheet_ops( adobe::name_t name, const adobe::any_regular_t& value );
    //
    /// This function handles file manipulation dirty status by sending
    /// it to the editor window to toggle the "save" button(s).
    ///
    /// \param  dirty_cell_name the name of the celll to set
    /// \param  dirty           whether or not the file is dirty
    //
    void _monitor_file_dirty( adobe::name_t dirty_cell_name, bool dirty );

public:
    //
    /// The getInstance method returns a pointer to the single application_t
    /// instance. If the application_t cannot be initialized then NULL is
    /// returned. Initialization will fail if:
    /// <ul>
    ///  <li>The editor.adm and editor.eve files cannot be found</li>
    ///  <li>The editor.adm and editor.eve files do not parse</li>
    ///  <li>OS-specific initialization fails</li>
    /// </ul>
    ///
    /// \return a pointer to the single application_t instance.
    /// \sa     os_initialize
    //
    static application_t* getInstance();
    //
    /// Load the main editor GUI and enter the main-loop. This function
    /// will not return until it's time to quit.
    //
    void run();
    //
    /// Load the dialog from disk, and tell the _holder_m to show it. This
    /// function also updates the contents of the Eve text area to match what
    /// was loaded from disk. The window loaded is specified by set_eve_file.
    ///
    /// \sa     set_eve_file
    //
    void display_window();
    //
    /// Forget about the existing sheet and load one from disk. This also
    /// (re)opens the dialog. The sheet loaded is specified by set_adam_file.
    ///
    /// \sa     set_adam_file
    //
    void load_sheet();
    //
    /// Remove any frames which have been drawn around the widgets.
    //
    void clear_window_frames();
    //
    /// Draw frames around all widgets.
    //
    void frame_window();
    //
    /// Runs the currently selected Adam/Eve pair as a modal dialog
    /// using the modal dialog interface API suite.
    //
    void run_current_as_modal();
    //
    /// Set the file name of the Eve file to load. This does not actually
    /// load the file or display the window, call display_window to make
    /// that happen.
    ///
    /// \param  file_name   the name of the Eve file to load.
    /// \sa     display_window
    //
    void set_eve_file( const boost::filesystem::path& file_name );
    //
    /// Set the file name of the Adam file to load. This does not actually
    /// load the file or update the sheet, call load_sheet to make that
    /// happen.
    ///
    /// \param  file_name   the name of the Adam file to load.
    /// \sa     load_sheet
    //
    void set_adam_file( const boost::filesystem::path& file_name );
    //
    /// Set the directory which is used to load application resources
    /// (such as the editor.adm and editor.eve files).
    ///
    /// \param  res_path    the directory to load application
    ///             resources from.
    //
    void set_resource_directory( const boost::filesystem::path& res_path );
    //
    /// Set the size of any dialogs which are going to be created after
    /// this call.
    ///
    /// \param  s   the new size to make dialogs.
    //
    void set_dialog_size( size_enum_t s );
    //
    /// Display an error message in the error tab of the editor palette
    ///
    /// \param  msg the message to post.
    //
    void display_error( const std::string& msg ); 
    //
    /// Get the line at the given offset on the given file
    ///
    /// \param  file the file
    /// \param  line_start_position the position
    std::string format_stream_error(adobe::file_buffer_t& file, std::streampos line_start_position);
};

/****************************************************************************************************/

//
/// Perform OS-specific initialization tasks, such as registering menus. This
/// function should also set the resource directory used by application_t.
///
/// \param  theApp  the instance of application_t to call into when a menu item is
///         selected, or a system event is received.
/// \return true if initialization was successful, false otherwise.
//
bool os_initialize( application_t* theApp );

/****************************************************************************************************/

//
/// Enter the OS/toolkit main-loop. This function should return when it's time
/// to quit.
//
void os_mainloop(application_t& theApp);

/****************************************************************************************************/

//
/// Exit the OS/toolkit main-loop. This function should only be called after
/// os_mainloop has been called.
//
void os_end_mainloop();

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
