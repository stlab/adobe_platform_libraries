/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/modal_dialog_interface.hpp>

#if ADOBE_PLATFORM_MAC
    #include <adobe/macintosh_carbon_safe.hpp>
#elif ADOBE_PLATFORM_WIN
    #define WINDOWS_LEAN_AND_MEAN 1

    #include <windows.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
extern "C" void tss_cleanup_implemented()
{ }
#endif

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const char* eve_view_definition_g =
"/*"
"   Copyright 2005-2007 Adobe Systems Incorporated"
"   Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt"
"   or a copy at http://stlab.adobe.com/licenses.html)"
"*/"
""
"layout mini_image_size"
"{"
" interface:"
"   percent: false;"
"       "
"   view dialog (name: 'Mini Image Size')"
"   {"
"       column()"
"       {"
"           overlay()"
"           {"
"               panel(bind: @percent, value: true)"
"               {"
"                   edit_number(name: 'Height:', bind: @height_percent, digits: 7);"
"                   edit_number(name: 'Width:', bind: @width_percent, digits: 7);"
"               }"
"               panel(bind: @percent, value: false)"
"               {"
"                   edit_number(name: 'Height:', bind: @height_pixels, digits: 7);"
"                   edit_number(name: 'Width:', bind: @width_pixels, digits: 7);"
"               }"
"           }"
"           checkbox(name: 'Constrain Proportions', bind: @constrain);"
"           checkbox(name: 'Percent', bind: @percent);"
"       }"
"       column()"
"       {"
"           button(name: 'OK', action: @ok, default: true);"
"           button(name: 'Cancel', action: @cancel, cancel: true);"
"       }"
"   }"
"}";

/****************************************************************************************************/

const char* adam_model_definition_g =
"/*"
"   Copyright 2005-2007 Adobe Systems Incorporated"
"   Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt"
"   or a copy at http://stlab.adobe.com/licenses.html)"
"*/"
""
"sheet mini_image_size"
"{"
"input:"
"   original_width      : 5 * 300;"
"   original_height     : 7 * 300;"
""
"interface:"
"   constrain           : true;"
""
"   width_pixels        : original_width    <== round(width_pixels);"
"   height_pixels       : original_height   <== round(height_pixels);"
"   width_percent;"
"   height_percent;"
""
"logic:"
"   relate {"
"       width_pixels    <== round(width_percent * original_width / 100);"
"       width_percent   <== width_pixels * 100 / original_width;"
"   }"
"   "
"   relate {"
"       height_pixels   <== round(height_percent * original_height / 100);"
"       height_percent  <== height_pixels * 100 / original_height;"
"   }"
""
"   when (constrain) relate {"
"       width_percent   <== height_percent;"
"       height_percent  <== width_percent;"
"   }"
""
"output:"
"   result <== { height: height_pixels, width: width_pixels };"
"}";

/****************************************************************************************************/

bool always_true(adobe::name_t /*name*/, const adobe::any_regular_t& /*value*/)
{
    return true;
}

/****************************************************************************************************/

adobe::dialog_result_t do_test( std::istream&   eve_stream,
                                std::istream&   adam_stream)
{
    adobe::modal_dialog_t dialog;

    dialog.callback_m = &always_true;

    return dialog.go(eve_stream, adam_stream);
}

/****************************************************************************************************/

#if ADOBE_PLATFORM_MAC
static OSStatus AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );

bool os_init()
{
    static IBNibRef                 sNibRef;
    static const EventTypeSpec      kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };

    OSStatus err(noErr);

    err = ::CreateNibReference( CFSTR("modal_dialog_interface"), &sNibRef );
    if (err != noErr) return false;

    err = ::SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
    if (err != noErr) return false;

    ::InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
                                    GetEventTypeCount( kAppEvents ), kAppEvents,
                                    0, NULL );
    //
    // Register this app as an Appearance Client
    //
    ::RegisterAppearanceClient();

    return true;
}

/****************************************************************************************************/

static OSStatus AppEventHandler(EventHandlerCallRef /*inCaller*/, EventRef /*inEvent*/, void* /*inRefcon*/)
{
    //system_beep();
    return eventNotHandledErr;
}

#endif

/****************************************************************************************************/

#if ADOBE_PLATFORM_WIN
bool os_init()
{
    return true;
}
#endif

/****************************************************************************************************/

}// namespace

/****************************************************************************************************/

#if ADOBE_PLATFORM_WIN
int __stdcall WinMain( HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char** argv)
#endif
try
{
    if (!os_init()) throw std::runtime_error("OS initialization failed");

#if ADOBE_PLATFORM_MAC
    if (argc > 2)
    {
        std::string db_name(argv[2]);
        boost::filesystem::path eve_path((db_name + ".eve").c_str(), boost::filesystem::native);
        boost::filesystem::path adam_path((db_name + ".adm").c_str(), boost::filesystem::native);

        boost::filesystem::ifstream eve_stream(eve_path);
        boost::filesystem::ifstream adam_stream(adam_path);

        //std::cerr << "using file set \"" << db_name << '"' << std::endl;

        do_test(eve_stream, adam_stream);
    }
    else
#endif
    {
        std::stringstream eve_stream;
        std::stringstream adam_stream;

        eve_stream << eve_view_definition_g;
        adam_stream << adam_model_definition_g;

        //std::cerr << "using builtin file set" << std::endl;

        do_test(eve_stream, adam_stream);
    }

    return 0;
}
catch(const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;
    return 1;
}
catch(...)
{
    std::cerr << "Exception: Unknown"<< std::endl;
    return 1;
}
