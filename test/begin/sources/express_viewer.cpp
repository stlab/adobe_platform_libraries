/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <fstream>
#include <sstream>
#include <cmath>

#include <boost/bind.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>
#include <boost/gil/image.hpp>

#include <adobe/adam_evaluate.hpp>
#include <adobe/adam_parser.hpp>
#include <adobe/algorithm/clamp.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/file_slurp.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/modal_dialog_interface.hpp>
#include <adobe/future/resources.hpp>
#include <adobe/future/widgets/headers/factory.hpp>
#include <adobe/future/widgets/headers/factory.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/keyboard.hpp>
#include <adobe/localization.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>
#include <adobe/xstring.hpp>

#ifndef NDEBUG
    #include <iostream>
    #include <adobe/adam_parser.hpp>
    #include <adobe/istream.hpp>
#endif

#if ADOBE_PLATFORM_MAC
    #include <adobe/future/widgets/headers/platform_metrics.hpp>
#endif

#ifdef ADOBE_STD_SERIALIZATION
    #include <sstream>
    #include <adobe/iomanip_asl_cel.hpp>
#endif

#include "express_viewer.hpp"
#include "report_exception.hpp"

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const adobe::static_name_t  value_key_g                    ( "value" );
const adobe::static_name_t  version_info_name_g            ( "version_info" );
const adobe::static_name_t  update_adam_name_g             ( "update_adam" );
const adobe::static_name_t  update_eve_name_g              ( "update_eve" );
const adobe::static_name_t  save_adam_name_g               ( "save_adam" );
const adobe::static_name_t  save_eve_name_g                ( "save_eve" );
const adobe::static_name_t  clear_window_action_g          ( "clear_frames" );
const adobe::static_name_t  frame_widgets_action_g         ( "frame_widgets" );
const adobe::static_name_t  editor_adam_cell_g             ( "adam_source" );
const adobe::static_name_t  editor_eve_cell_g              ( "eve_source" );
const adobe::static_name_t  editor_adam_dirty_cell_g       ( "adam_dirty" );
const adobe::static_name_t  editor_eve_dirty_cell_g        ( "eve_dirty" );
const adobe::static_name_t  editor_error_cell_g            ( "errors" );
const adobe::static_name_t  editor_results_cell_g          ( "results" );
const adobe::static_name_t  editor_visible_tab_cell_g      ( "editor_tab_group" );
const adobe::static_name_t  editor_key_errors_tab_g        ( "errors" );
const adobe::static_name_t  editor_key_results_tab_g       ( "results" );

const adobe::static_name_t  editor_action_ls_inspect       ( "ls_inspect" );
const adobe::static_name_t  editor_action_ls_set           ( "ls_set" );
const adobe::static_name_t  editor_action_pm_dump          ( "pm_dump" );
const adobe::static_name_t  editor_action_pm_evaluate      ( "pm_evaluate" );
const adobe::static_name_t  editor_action_pm_set           ( "pm_set" );
const adobe::static_name_t  editor_ls_expression           ( "ls_expression" );
const adobe::static_name_t  editor_ls_results              ( "ls_results" );
const adobe::static_name_t  editor_ls_to_cell              ( "ls_to_cell" );
const adobe::static_name_t  editor_pm_expression           ( "pm_expression" );
const adobe::static_name_t  editor_pm_results              ( "pm_results" );
const adobe::static_name_t  editor_pm_to_cell              ( "pm_to_cell" );

/****************************************************************************************************/

const char* empty_layout_g =
"layout my_dialog\n"
"{\n"
"    view dialog(name: localize(\"<xstr id='my_dialog_name'>My Dialog</xstr>\"))\n"
"    {\n"
"        slider(bind: @my_value, format: {first: 0, last: 100});\n"
"        edit_number(name: 'Value:', bind: @my_value, format: '#', alt: 'Alters the value of the slider');\n"
"        button (items: [\n"
"                           { name: localize(\"<xstr id='ok'>OK</xstr>\"), action: @ok, bind: @result, alt: 'Perform the command with the current settings' },\n"
"                           { name: localize(\"<xstr id='reset'>Reset</xstr>\"), action: @reset, modifiers: @opt, alt: 'Reset the dialog settings' }\n"
"                       ]);\n"
"    }\n"
"}\n";

/****************************************************************************************************/

const char* empty_sheet_g =
"sheet my_sheet\n"
"{\n"
"interface:\n"
"   my_value: 42;\n"
"output:\n"
"   result <== { value: my_value };\n"
"}\n";


/****************************************************************************************************/

/*
    REVISIT (fbrereto) : The behavior map is a tree of behaviors with explicit ownership to the
                         root of the tree. In the case of the window update behavior, the root
                         of the tree is initialized at the first call to ech_window_update_behavior().
                         This used to take place when the first eve_client_holder needed it. The
                         problem with that, however, is that the tree will then be deleted first,
                         then the eve_client_holders by the application destructor (assuming there
                         still exist some eve_client_holders in the current window server). This
                         results in a nasty crash, as the static variable destructs and takes all
                         its children with it, then the eve_client_holders try to disconnect
                         themselves from the static variable that has since been destroyed already.
                         We make this call to ech_window_update_behavior here to have it registered
                         in the static initialization stack early so the application can destruct
                         before it does.
*/

//adobe::behavior_t& wub_dummy_g(adobe::ech_window_update_behavior());

// same with adobe::keyboard_t
adobe::keyboard_t& kbd_dummy_g(adobe::keyboard_t::get());

/****************************************************************************************************/

inline std::string simple_xstring_lookup(const std::string& src)
{
    // ZString hack: Clip to the first '='
    if (src[0] == '$' && src[1] == '$' && src[2] == '$')
    {
        std::string::size_type pos(src.find('='));

        return pos == std::string::npos ?
            std::string() :
            src.substr(pos + 1, std::string::npos);
    }

    return adobe::xstring(src);
}

/****************************************************************************************************/

adobe::any_regular_t ps_finalize(const adobe::array_t& arg_set)
{
    if (arg_set.empty())
        throw std::runtime_error("ps_finalize usage: ps_finalize(image)");

    return arg_set[0];
}

/****************************************************************************************************/

adobe::any_regular_t channel_invert(const adobe::array_t& arg_set)
{
    if (arg_set.size() < 2)
        throw std::runtime_error("channel_invert usage: channel_invert(image, params)");

    boost::gil::rgba8_image_t filtered_image(arg_set[0].cast<boost::gil::rgba8_image_t>());
    boost::gil::rgba8_view_t::iterator iter(filtered_image._view.begin());
    boost::gil::rgba8_view_t::iterator last(filtered_image._view.end());
    const adobe::dictionary_t&         param_set(arg_set[1].cast<adobe::dictionary_t>());

    bool invert_red(get_value(param_set, adobe::static_name_t("invert_red")).cast<bool>());
    bool invert_green(get_value(param_set, adobe::static_name_t("invert_green")).cast<bool>());
    bool invert_blue(get_value(param_set, adobe::static_name_t("invert_blue")).cast<bool>());

    for (; iter != last; ++iter)
    {
        boost::gil::rgba8_pixel_t pixel = *iter;

        if (invert_red)
            pixel[0] = 255 - pixel[0];

        if (invert_green)
            pixel[1] = 255 - pixel[1];

        if (invert_blue)
            pixel[2] = 255 - pixel[2];

        *iter = pixel;
    }

    return adobe::any_regular_t(filtered_image);
}

/****************************************************************************************************/

adobe::any_regular_t pi_filter_op(const adobe::array_t& arg_set)
{
    if (arg_set.size() < 2)
        throw std::runtime_error("pi_filter_op usage: pi_filter_op(image, params)");

    return adobe::any_regular_t(arg_set[0]);
}

/****************************************************************************************************/

adobe::any_regular_t serialize_(const adobe::array_t& arg_set)
{
    if (arg_set.empty())
        throw std::runtime_error("serialize usage: serialize(any_serializable)");

    std::stringstream stream;

    stream << adobe::begin_asl_cel << arg_set[0] << adobe::end_asl_cel;

    return adobe::any_regular_t(stream.str());
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

application_t::application_t() :
    _sheet_m( 0 ),
    _behavior_m( false ),
    _holder_m( 0 ),
    _editor_behavior_m( false ),
    _dialog_size_m( size_normal_s ),
    _initialized( false )
{
    kbd_dummy_g.get(); // force a reference to this global.
    
    _vm_lookup_m.attach_to(_editor_sheet_m);
    _vm_lookup_m.attach_to(_editor_sheet_m.machine_m);

    _vm_lookup_m.insert_array_function(adobe::static_name_t("ps_finalize"), ps_finalize);
    _vm_lookup_m.insert_array_function(adobe::static_name_t("channel_invert"), channel_invert);
    _vm_lookup_m.insert_array_function(adobe::static_name_t("pi_filter_op"), pi_filter_op);
    _vm_lookup_m.insert_array_function(adobe::static_name_t("serialize"), serialize_);
}

/****************************************************************************************************/

application_t::~application_t()
{
    //
    // We have to delete _holder_m before _sheet_m.
    //
    if ( _holder_m )
        delete _holder_m;

    if ( _sheet_m )
        delete _sheet_m;
}

/****************************************************************************************************/

bool application_t::_initialize()
{
    if ( _initialized ) return true;

    if (!adobe::localization_ready())
        adobe::localization_register(&simple_xstring_lookup);

#if ADOBE_PLATFORM_MAC
    // This is a lame hack to inject the mac metrics glossary
    // into the context stack before any more work is done.
    // Otherwise it will get injected when it is first used,
    // which is when the first widget measurement will take
    // place, which is also *after* the glossary call below.
    // The glossary call will fall out of scope at the end
    // of this function, but instead of popping the glossary
    // it'll pop the injected mac metrics glossary, which is
    // not what we want.

    adobe::widget_metrics("", adobe::dictionary_t());
#endif

    //
    // The os_initialize function is implemented in win/main.cpp or
    // mac/main.cpp.
    //
    if ( !os_initialize( this ) )
        return false;

    //
    // Load up the default string glossary for localization
    //
    adobe::file_slurp<char>     glossary_slurp(adobe::find_resource(boost::filesystem::path("glossary.xstr")));
    adobe::xstring_context_t    context(glossary_slurp.begin(), glossary_slurp.end(),
                                        adobe::line_position_t( "glossary.xstr" ) );

    glossary_slurp.release();

    //
    // We need to load the editor.adm and editor.eve files.
    //
    bfs::path editor_adm(adobe::find_resource(boost::filesystem::path("editor.adm")));
    bfs::path editor_eve(adobe::find_resource(boost::filesystem::path("editor.eve")));
    bfs::ifstream stream( editor_adm );

    //
    // Verify that we can open the editor.adm resource file. If we can then
    // parse the file into _editor_sheet_m.
    //
    if ( !stream.is_open() )
        throw std::runtime_error( "Can't open file: \"" + editor_adm.string() + "\"" );

    adobe::parse( stream, adobe::line_position_t( editor_adm.string().c_str() ), bind_to_sheet( _editor_sheet_m ) );

    _editor_sheet_m.update();

    //
    // The Eve view lives in this static auto pointer. This is also where
    // we bind the _editor_op function to the loaded widgets.
    //

    bfs::ifstream                   eve_view_stream( editor_eve );
    adobe::file_buffer_t            editor_eve_fbuffer;
    line_position_t::getline_proc_t getline_proc(new line_position_t::getline_proc_impl_t(boost::bind(&application_t::format_stream_error, boost::ref(*this), boost::ref(editor_eve_fbuffer), _2)));

    _editor_holder_m = adobe::make_view(adobe::name_t(editor_eve.string().c_str()),
                                        getline_proc,
                                        eve_view_stream,
                                        _editor_sheet_m,
                                        _editor_behavior_m,
                                        boost::bind(
                                            &application_t::_editor_op,
                                            this, _1, _2 ),
                                        size_small_s);

    //
    // Bind the monitor_dirty functions with the editor window
    //

    _eve_file_m.monitor_dirty(boost::bind(&application_t::_monitor_file_dirty, boost::ref(*this), editor_eve_dirty_cell_g, _1));
    _adam_file_m.monitor_dirty(boost::bind(&application_t::_monitor_file_dirty, boost::ref(*this), editor_adam_dirty_cell_g, _1));

    //
    // Bind the monitor_disk_update functions to their respective reload operations
    //

    _eve_file_m.monitor_disk_update(boost::bind(&application_t::display_window, boost::ref(*this)));
    _adam_file_m.monitor_disk_update(boost::bind(&application_t::load_sheet, boost::ref(*this)));

    //
    // Update the sheet and show the GUI.
    //
    _editor_sheet_m.update();
    _editor_holder_m->eve_m.evaluate(adobe::eve_t::evaluate_nested);
    _editor_holder_m->show_window_m();

#if ADOBE_PLATFORM_MAC
    //
    // set the ui_core error reporting callback
    //

    set_error_handler(boost::bind(&application_t::display_error, boost::ref(*this), _1));
#endif

    _initialized = true;

    return true;
}

/****************************************************************************************************/

void application_t::_button_notifier( const adobe::name_t& name, const adobe::any_regular_t& value )
{
    if (name == adobe::static_name_t("ok"))
    {
#if defined(ADOBE_STD_SERIALIZATION)
        std::stringstream result;

        result << adobe::begin_asl_cel << value << adobe::end_asl_cel;
        //
        // Update the adam sheet with this new output.
        //
            _editor_sheet_m.set( editor_results_cell_g, adobe::any_regular_t( result.str() ) );
            _editor_sheet_m.update();
            _editor_holder_m->layout_sheet_m.set( editor_visible_tab_cell_g, adobe::any_regular_t( editor_key_results_tab_g ) );
#endif
    }
}

/****************************************************************************************************/

void application_t::_monitor_file_dirty( adobe::name_t dirty_cell_name, bool dirty )
{
    _editor_sheet_m.set( dirty_cell_name, adobe::any_regular_t( dirty ) );

    _editor_sheet_m.update();
}

/****************************************************************************************************/

void application_t::_sheet_ops( adobe::name_t name, const adobe::any_regular_t& value )
{
    if (_sheet_m == 0)
        return;

    adobe::dictionary_t value_dictionary(get_value(value.cast<adobe::dictionary_t>(), adobe::static_name_t("value")).cast<adobe::dictionary_t>());

    if (name == editor_action_pm_evaluate ||
        name == editor_action_pm_set ||
        name == editor_action_pm_dump)
    {
#if defined(NDEBUG) || !defined(ADOBE_STD_SERIALIZATION)
        display_error("Property model evaluation not available in Release or non-serialization builds.");
        return;
#else
        std::string pm_expression;
        std::string pm_cell;

        get_value(value_dictionary, editor_pm_expression, pm_expression);
        get_value(value_dictionary, editor_pm_to_cell, pm_cell);

        adobe::array_t       parsed_pm_expression(adobe::parse_adam_expression(pm_expression));

        if (name == editor_action_pm_evaluate)
        {
            std::stringstream    stream;
            adobe::any_regular_t evaluated_pm_expression(_sheet_m->inspect(parsed_pm_expression));

            stream << adobe::begin_asl_cel << evaluated_pm_expression << adobe::end_asl_cel;

            _editor_sheet_m.set( editor_pm_results, adobe::any_regular_t( stream.str() ) );
            _editor_sheet_m.update();
        }
        else if (name == editor_action_pm_dump)
        {
            std::stringstream stream;

            stream << adobe::begin_asl_cel << parsed_pm_expression << adobe::end_asl_cel;

            _editor_sheet_m.set( editor_pm_results, adobe::any_regular_t( stream.str() ) );
            _editor_sheet_m.update();
        }
        else if (name == editor_action_pm_set)
        {
            adobe::any_regular_t evaluated_pm_expression(_sheet_m->inspect(parsed_pm_expression));

            if (pm_cell == std::string())
            { display_error("You must enter a cell name to which the value will be set"); }
            else
            {
                sheet_t::monitor_value_t set_cell_from_controller_proc = 
                    implementation::obtain_set_cell_from_controller_proc(
                        *_sheet_m, adobe::name_t(pm_cell.c_str()));

                implementation::touch_set_update(*_sheet_m,
                                                 set_cell_from_controller_proc,
                                                 evaluated_pm_expression,
                                                 touch_set_t(),
                                                 _holder_m->top_client_holder().root_behavior_m);

                _editor_sheet_m.set( editor_pm_results, adobe::any_regular_t( std::string("cell value updated.") ) );
                _editor_sheet_m.update();
            }
        }
#endif
    }
    else if (name == editor_action_ls_inspect ||
             name == editor_action_ls_set)
    {
#if defined(NDEBUG) || !defined(ADOBE_STD_SERIALIZATION)
        display_error("Layout sheet evaluation not available in Release or non-serialization builds.");
        return;
#else
        std::string ls_expression;
        std::string ls_cell;

        get_value(value_dictionary, editor_ls_expression, ls_expression);
        get_value(value_dictionary, editor_ls_to_cell, ls_cell);

        if (ls_cell == std::string())
        {
            display_error("You must enter a cell name to which the value will be set");
        }
        else if (name == editor_action_ls_inspect)
        {
            std::stringstream stream;

            stream << adobe::begin_asl_cel << _holder_m->top_client_holder().layout_sheet_m[adobe::name_t(ls_cell.c_str())] << adobe::end_asl_cel;

            _editor_sheet_m.set( editor_ls_results, adobe::any_regular_t( stream.str() ) );
            _editor_sheet_m.update();
        }
        else if (name == editor_action_ls_set)
        {
            adobe::any_regular_t evaluated_ls_expression(_sheet_m->inspect(adobe::parse_adam_expression(ls_expression)));
            adobe::sheet_t::monitor_value_t      set_cell_from_controller_proc = 
                implementation::obtain_set_cell_from_controller_proc(
                    _holder_m->top_client_holder().layout_sheet_m, 
                    adobe::name_t(ls_cell.c_str()));

            implementation::touch_set_update(_holder_m->top_client_holder().layout_sheet_m,
                                             set_cell_from_controller_proc,
                                             evaluated_ls_expression,
                                             touch_set_t(),
                                             _holder_m->top_client_holder().root_behavior_m);
    
            _editor_sheet_m.set( editor_ls_results, adobe::any_regular_t( std::string("layout sheet cell value updated.") ) );
            _editor_sheet_m.update();
        }
#endif
    }
    else
    {
        display_error("Unknown sheet operation.");
    }
/*
    else if ( name == inspector_dump_name_g )
    {
#if defined(ADOBE_STD_SERIALIZATION)
        stream << adobe::begin_pdf << expression << adobe::end_pdf;
#else
        system_beep();
#endif
    }
    else if ( name == layout_sheet_inspect_g && _holder_m && _editor_sheet_m )
    {
        std::stringstream stream;
        adobe::name_t     cell_name(result.c_str());

#if defined(ADOBE_STD_SERIALIZATION)
        stream << adobe::begin_pdf << _holder_m->inspect_top_layout_sheet(cell_name) << adobe::end_pdf;
#endif
        //
        // Now set whatever we just streamed into the
        // inspector cell.
        //
        _editor_sheet_m.set( editor_inspector_result_cell_g, adobe::any_regular_t( stream.str() ) );
        _editor_sheet_m.update();
    }
*/
}

/****************************************************************************************************/

void application_t::_editor_op( adobe::name_t name, const adobe::any_regular_t& value )
{
    try
    {
        std::string result;

        get_value(value.cast<adobe::dictionary_t>(), value_key_g, result);

        if (name == editor_action_pm_evaluate ||
            name == editor_action_pm_dump ||
            name == editor_action_pm_set ||
            name == editor_action_ls_inspect ||
            name == editor_action_ls_set)
        {
            _sheet_ops(name, value);
        }
        else if ( name == version_info_name_g )
        {
            std::stringstream stream;

            stream << "Adobe Begin " << ADOBE_VERSION_MAJOR
                   << "." << ADOBE_VERSION_MINOR << "." << ADOBE_VERSION_SUBMINOR << "\n";
            stream << "Adobe contributions by\n";
            stream << "\tFoster Brereton, Mat Marcus, Sean Parent,\n";
            stream << "\tEric Berdahl, Lubomir Bourdev, Hailin Jin,\n";
            stream << "\tJon Reid, Mark Ruzon\n";
            stream << "Opensource community contributions by\n";
            stream << "\tDavid Catmull, Jamie Gadd, Peter Kummel,\n";
            stream << "\tTobias Schwinger, Niki Spahiev, Ralph Thomas,\n";
            stream << "\tThomas Witt\n";
            stream << "Using Adobe Source Library v. " << ADOBE_VERSION_MAJOR
                   << "." << ADOBE_VERSION_MINOR << "." << ADOBE_VERSION_SUBMINOR << "\n";
            stream << "Using Boost v. " << BOOST_VERSION / 100000 << "."
                   << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << "\n";

            _editor_sheet_m.set( editor_results_cell_g, adobe::any_regular_t( stream.str() ) );
            _editor_sheet_m.update();
            _editor_holder_m->layout_sheet_m.set( editor_visible_tab_cell_g, adobe::any_regular_t( editor_key_results_tab_g ) );
        }
        else if ( name == update_adam_name_g )
        {
            _adam_file_m.set_contents(result.begin(), result.end());

            load_sheet();
        }
        else if ( name == update_eve_name_g )
        {
            _eve_file_m.set_contents(result.begin(), result.end());

            display_window();
        }
        else if ( name == save_adam_name_g )
        {
            _adam_file_m.set_contents(result.begin(), result.end());

            _adam_file_m.save();
        }
        else if ( name == save_eve_name_g )
        {
            _eve_file_m.set_contents(result.begin(), result.end());

            _eve_file_m.save();
        }
        else
        {
            display_error("Unknown action.");
        }
    }
    catch ( ... )
    {
        adobe::report_exception();
    }
}

/****************************************************************************************************/

application_t* application_t::getInstance()
try
{
    static application_t theApp;

    if ( theApp._initialize() ) return &theApp;

    std::cerr << "App initialization failed." << std::endl;

    return 0;
}
catch (std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;
    return 0;
}
catch (...)
{
    std::cerr << "Exception: Unknown" << std::endl;
    return 0;
}

/****************************************************************************************************/

void application_t::run()
{
    //
    // We should already be initialized by the time we get to this point, so we need
    // to load the xstring glossary as part of the current context. On Mac, too, we
    // need to specify the Mac metrics glossary.
    //
    //
    // Load up the default string glossary for localization
    //
    adobe::file_slurp<char>     glossary_slurp(adobe::find_resource(boost::filesystem::path("glossary.xstr")));
    adobe::xstring_context_t    context(glossary_slurp.begin(), glossary_slurp.end(),
                                        adobe::line_position_t( "glossary.xstr" ) );

    glossary_slurp.release();

    //
    // fill the contents of the adam and eve file buffers with the "null dialog"
    //
    _eve_file_m.set_contents(empty_layout_g, empty_layout_g + std::strlen(empty_layout_g));
    _adam_file_m.set_contents(empty_sheet_g, empty_sheet_g + std::strlen(empty_sheet_g));

    load_sheet();

    //
    // Run the main event loop for the app
    //
    os_mainloop(*this);
}

/****************************************************************************************************/

void application_t::display_window()
{
    //
    // We can't display the window without a loaded sheet
    //
    if ( _sheet_m == 0 )
        return;

    {
        //
        // Load the Eve file into the editor window.
        //
        _editor_sheet_m.set( editor_eve_cell_g, adobe::any_regular_t( _eve_file_m.as_string() ) );
        _editor_sheet_m.set( editor_error_cell_g, adobe::any_regular_t( std::string() ) );
        _editor_sheet_m.update();
    }

    //
    // Create a new window server if we don't already have one.
    //
    if ( !_holder_m )
    {
        //
        // Note that the sheet is passed straight to the window_server
        // here. This is why the window server always has to be
        // destroyed before the sheet.
        //
        //const bfs::path& path = _eve_file_m.directory_path();
        //const std::string& dir = path.string();
        _holder_m = new adobe::window_server_t( *_sheet_m, _behavior_m );
        _holder_m->set_action_fallback( boost::bind(
            &application_t::_button_notifier, this, _1, _2 ) );
    }

    //
    // This causes the holder to load the Eve file we pass it. We tell the
    // holder the directory (above) so that when it encounters an @dialog
    // command it knows the directory to look for the dialog in.
    //
    std::stringstream stream( _eve_file_m.as_string() );
    line_position_t::getline_proc_t getline_proc(new line_position_t::getline_proc_impl_t(boost::bind(&application_t::format_stream_error, boost::ref(*this), boost::ref(_eve_file_m), _2)));
    
    try
    {
        _holder_m->push_back( stream, _eve_file_m.get_path(), getline_proc, _dialog_size_m );
    }
    catch(const adobe::stream_error_t&)
    {
        adobe::report_exception();

        return;
    }
    catch ( ... )
    {
        throw;
    }
}

/****************************************************************************************************/

std::string application_t::format_stream_error(adobe::file_buffer_t& file, std::streampos line_start_position)
{
    using namespace std;

    string contents(file.as_string());

    assert(string::size_type(line_start_position) - 1 <= contents.size());

    string::iterator first = contents.begin() + (string::size_type(line_start_position) - 1);
    string::iterator cr_pos = std::find(first, contents.end(), '\r');
    string::iterator lf_pos = std::find(first, contents.end(), '\n');

    string::iterator last((std::min)(cr_pos, lf_pos));

    return string(first, last);
}

/****************************************************************************************************/

void application_t::load_sheet()
{
    //
    // We can't make any changes to the sheet without changing the window,
    // so we delete the current window and then reload it once we've loaded
    // the new sheet.
    //
    if ( _holder_m )
    {
        delete _holder_m;
        _holder_m = 0;
    }

    {
        //
        // Show the adam file in the editor window.
        //
        _editor_sheet_m.set( editor_adam_cell_g, adobe::any_regular_t( _adam_file_m.as_string() ) );
        _editor_sheet_m.set( editor_error_cell_g, adobe::any_regular_t( std::string() ) );
        _editor_sheet_m.update();
    }

    //
    // Destroy any existing sheet and create a new one.
    //
    if ( _sheet_m )
        delete _sheet_m;

    _sheet_m = new adobe::sheet_t();

    _vm_lookup_m.attach_to(*_sheet_m);
    _vm_lookup_m.attach_to(_sheet_m->machine_m);

    _behavior_m = adobe::behavior_t(false);

    //
    // Open the Adam file and load it into our sheet.
    //
    std::stringstream stream( _adam_file_m.as_string() );
    line_position_t::getline_proc_t getline_proc(new line_position_t::getline_proc_impl_t(boost::bind(&application_t::format_stream_error, boost::ref(*this), boost::ref(_adam_file_m), _2)));

    try
    {
        adobe::parse( stream, adobe::line_position_t(adobe::name_t(_adam_file_m.file_name()), getline_proc), bind_to_sheet( *_sheet_m ) );
    }
    catch(const adobe::stream_error_t&)
    {
        adobe::report_exception();

        return;
    }
    catch ( ... )
    {
        throw;
    }

   _sheet_m->update();

    //
    // As we destroyed the window we need to recreate it, if we can.
    //
    display_window();
}

/****************************************************************************************************/

void application_t::clear_window_frames()
{
    if ( _holder_m ) _holder_m->dispatch_action( clear_window_action_g, adobe::any_regular_t() );
    else system_beep();
}

/****************************************************************************************************/

void application_t::frame_window()
{
    if ( _holder_m ) _holder_m->dispatch_action( frame_widgets_action_g, adobe::any_regular_t() );
    else system_beep();
}

/****************************************************************************************************/

bool always_break(adobe::name_t, const adobe::any_regular_t&)
{
    return true;
}

/****************************************************************************************************/

void application_t::run_current_as_modal()
{
    std::stringstream     adam_stream( _adam_file_m.as_string() );
    std::stringstream     eve_stream( _eve_file_m.as_string() );
    adobe::modal_dialog_t dialog;

    dialog.display_options_m = adobe::dialog_display_s;
    dialog.callback_m = &always_break;
    dialog.working_directory_m = _eve_file_m.directory_path();
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("ps_finalize"), ps_finalize);
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("channel_invert"), channel_invert);
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("pi_filter_op"), pi_filter_op);
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("serialize"), serialize_);

    adobe::dialog_result_t result(dialog.go(eve_stream, adam_stream));

    _editor_sheet_m.set(editor_results_cell_g, serialize_(adobe::array_t(1, adobe::any_regular_t(result.command_m))));
    _editor_sheet_m.update();
    _editor_holder_m->layout_sheet_m.set(editor_visible_tab_cell_g, adobe::any_regular_t(editor_key_results_tab_g));
}

/****************************************************************************************************/

void application_t::set_eve_file( const bfs::path& file_name )
{
    _eve_file_m.set_path(file_name);
}

/****************************************************************************************************/

void application_t::set_adam_file( const bfs::path& file_name )
{
    _adam_file_m.set_path(file_name);
}

/****************************************************************************************************/

void application_t::set_resource_directory( const bfs::path& res_path )
{
    static bool                      set_s(false);
    static adobe::resource_context_t context(res_path);

    if (set_s)
        throw std::runtime_error("This application is only set up to specify the resource directory once.");

    set_s = true;
}

/****************************************************************************************************/

void application_t::set_dialog_size( size_enum_t s )
{
    _dialog_size_m = s;
}

/****************************************************************************************************/

void application_t::display_error( const std::string& msg )
{
    {
        _editor_sheet_m.set( editor_error_cell_g, adobe::any_regular_t( msg ) );
        _editor_sheet_m.update();
        _editor_holder_m->layout_sheet_m.set( editor_visible_tab_cell_g, adobe::any_regular_t( editor_key_errors_tab_g ) );
    }
#ifndef NDEBUG
    std::cerr << msg << std::endl;
#endif

    system_beep();
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
