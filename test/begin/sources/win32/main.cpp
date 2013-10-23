/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/****************************************************************************************************/

#include "express_viewer.hpp"
#include "report_exception.hpp"
#include <adobe/future/widgets/headers/display.hpp>
#include "resources.h"
#include <adobe/future/windows_cast.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <adobe/xstring.hpp>

#include <boost/filesystem/convenience.hpp>

#include <adobe/future/behavior.hpp>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Commctrl.h>
#include <tchar.h>

#include <sstream>
#include <ostream>
#include <string>

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
namespace boost {
	void tss_cleanup_implemented()
{ }
}
#endif


/****************************************************************************************************/

namespace {

/****************************************************************************************************/

enum
{
    ADOBE_ABOUT = 40001,
    ADOBE_QUIT,
    ADOBE_REFRESH_VIEW,
    ADOBE_REFRESH_SHEET,
    ADOBE_NORMAL_DIALOG_SIZE,
    ADOBE_SMALL_DIALOG_SIZE,
    ADOBE_MINI_DIALOG_SIZE,
    ADOBE_SHOW_WIDGET_FRAMES,
    ADOBE_CLEAR_WIDGET_FRAMES,
    ADOBE_SERIALIZE_WIDGETS,
    ADOBE_RUN_MODAL,

    ADOBE_LOCALIZATION_ENUS,
    ADOBE_LOCALIZATION_DEDE,
    ADOBE_LOCALIZATION_JAJP,
    ADOBE_LOCALIZATION_KOKR,
    ADOBE_LOCALIZATION_PGPG
};

/****************************************************************************************************/

void clip_quotes(std::string& str)
{
    //
    // Sometimes GetCommandLine puts double quotes at the beginning and
    // end of the command line (this doesn't always happen, not if the
    // program was started from the prompt instead of the GUI).
    //
    if( str[0] == '"' )
        str = str.substr( 1, str.size() - 2 );
}

/****************************************************************************************************/

void open_document(adobe::application_t* app, const std::string& filename)
{
    boost::filesystem::path     file( filename, boost::filesystem::native );
    std::string                 extension( boost::filesystem::extension( file ) );

    if ( extension == ".eve")
        app->set_eve_file( file );
    else if ( extension == ".adm")
        app->set_adam_file( file );
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

//
/// Main window event handler
///
/// \param  window  the window associated with the event.
/// \param  message the Windows event type.
/// \param  wParam  the pointer parameter.
/// \param  lParam  the integer parameter.
///
/// \return zero, or whatever the default window handler returns.
//
LRESULT CALLBACK main_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    application_t*  app(hackery::cast<application_t*>(::GetWindowLongPtr(window, GWLP_USERDATA)));
    bool                handled(true);

    if (message == WM_CLOSE)
    {
        ::PostQuitMessage(0);
    }
    if (message == WM_COMMAND)
    {
        //HMENU menu(::GetMenu(window));
        WORD    command(LOWORD(wParam));

        switch(command)
        {
            case ADOBE_ABOUT:
                ::MessageBox(NULL, _T("Adobe Begin Copyright 2005-2007 Adobe Systems Incorporated"), _T("About Adobe Begin"), MB_OK);
                break;

            case ADOBE_QUIT:
                ::PostQuitMessage(0);
                break;

            case ADOBE_REFRESH_VIEW:
                app->display_window();
                break;

            case ADOBE_REFRESH_SHEET:
                app->load_sheet();
                break;

            case ADOBE_NORMAL_DIALOG_SIZE:
                app->set_dialog_size( size_normal_s );
                app->display_window();
                break;

            case ADOBE_SMALL_DIALOG_SIZE:
                app->set_dialog_size( size_small_s );
                app->display_window();
                break;

            case ADOBE_MINI_DIALOG_SIZE:
                app->set_dialog_size( size_mini_s );
                app->display_window();
                break;

#ifndef NDEBUG
            case ADOBE_SHOW_WIDGET_FRAMES:
                app->frame_window();
                break;

            case ADOBE_CLEAR_WIDGET_FRAMES:
                app->clear_window_frames();
                break;
#else
            case ADOBE_SHOW_WIDGET_FRAMES:
            case ADOBE_CLEAR_WIDGET_FRAMES:
                system_beep();
                break;
#endif

#if 0
            case ADOBE_SERIALIZE_WIDGETS:
                app->serialize_connections();
                break;
#endif

            case ADOBE_RUN_MODAL:
                app->run_current_as_modal();
                break;

            case ADOBE_LOCALIZATION_ENUS:
                adobe::implementation::top_frame().attribute_set_m.insert(
                    std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("en_US")));
                app->display_window();
                break;

            case ADOBE_LOCALIZATION_DEDE:
                adobe::implementation::top_frame().attribute_set_m.insert(
                    std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("de_DE")));
                app->display_window();
                break;

            case ADOBE_LOCALIZATION_JAJP:
                adobe::implementation::top_frame().attribute_set_m.insert(
                    std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("ja_JP")));
                app->display_window();
                break;

            case ADOBE_LOCALIZATION_KOKR:
                adobe::implementation::top_frame().attribute_set_m.insert(
                    std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("ko_KR")));
                app->display_window();
                break;

            case ADOBE_LOCALIZATION_PGPG:
                adobe::implementation::top_frame().attribute_set_m.insert(
                    std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("pg_PG")));
                app->display_window();
                break;

                default:
                handled = false; 
        }
    }
    else if (message == WM_DROPFILES)
    {
        HDROP               drop(reinterpret_cast<HDROP>(wParam));
        UINT                numfiles(::DragQueryFile(drop, 0xFFFFFFFF, NULL, 0));

        for (UINT i(0); i < numfiles; ++i)
        {
            UINT size_needed(::DragQueryFile(drop, i, NULL, 0));

            std::string buffer(size_needed + 1, 0);

            ::DragQueryFileA(drop, i, &buffer[0], size_needed + 1);

            std::string temp(&buffer[0]);

            if (temp.rfind(".adm") == temp.size() - 4)
                app->set_adam_file(boost::filesystem::path(temp.c_str(), boost::filesystem::native));
            else if (temp.rfind(".eve") == temp.size() - 4)
                app->set_eve_file(boost::filesystem::path(temp.c_str(), boost::filesystem::native));
        }

        ::DragFinish(drop);

        app->load_sheet();
    }
    else if (message == WM_DESTROY)
    {
    }
    else
    {
        handled = false;
    }
    //
    // Pass it to the default window procedure if we haven't handled it.
    //
    return handled ? 0 : DefWindowProcW(window, message, wParam, lParam);
}

/****************************************************************************************************/

void setup_main_window( application_t* app )
{
    // INIT ONCE all this stuff!
    INITCOMMONCONTROLSEX control_info = { sizeof(INITCOMMONCONTROLSEX), 0x0000FFFF };
    ::InitCommonControlsEx(&control_info);

    WNDCLASSW wc;

    const wchar_t* window_class(L"adobe_begin_main_window");

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = main_window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(RES_APP_ICON));
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = window_class;
    wc.lpszClassName = window_class;

    RegisterClassW(&wc);

    HWND root_window = ::CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME | WS_EX_COMPOSITED,
                                         window_class,
                                         L"Adobe Begin",
                                         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                         10, 10, 640, 480, NULL,
                                         ::CreateMenu(),
                                         ::GetModuleHandle(NULL),
                                         NULL);

    if (root_window == NULL) throw std::runtime_error("Could not open main window");

    ::ShowWindow(root_window, SW_NORMAL);
    ::DragAcceptFiles(root_window, TRUE);
    ::SetWindowLongPtrA(root_window, GWLP_USERDATA, hackery::cast<LONG>(app));

    // setup the menu system
    HMENU menu_bar(::GetMenu(root_window));
    HMENU app_menu(::CreatePopupMenu());
    HMENU view_options_menu(::CreatePopupMenu());
    HMENU localization_menu(::CreatePopupMenu());

    ::AppendMenu(app_menu, MF_STRING, ADOBE_ABOUT, _T("&About Adobe Begin"));
    ::AppendMenu(app_menu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(app_menu, MF_STRING, ADOBE_QUIT, _T("&Quit Adobe Begin"));

    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_REFRESH_VIEW, _T("&Refresh View"));
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_REFRESH_SHEET, _T("R&efresh Sheet"));
    ::AppendMenu(view_options_menu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_NORMAL_DIALOG_SIZE, _T("&Normal Dialog Size"));
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_SMALL_DIALOG_SIZE, _T("&Small Dialog Size"));
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_MINI_DIALOG_SIZE, _T("&Mini Dialog Size"));
    ::AppendMenu(view_options_menu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_SHOW_WIDGET_FRAMES, _T("&Show Widget Frames"));
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_CLEAR_WIDGET_FRAMES, _T("&Clear Widget Frames"));
    ::AppendMenu(view_options_menu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_SERIALIZE_WIDGETS, _T("&Serialize Widgets"));
    ::AppendMenu(view_options_menu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(view_options_menu, MF_STRING, ADOBE_RUN_MODAL, _T("&Run Modal"));

    ::AppendMenu(localization_menu, MF_STRING, ADOBE_LOCALIZATION_ENUS, _T("&Standard English (en-us)"));
    ::AppendMenu(localization_menu, MF_STRING, ADOBE_LOCALIZATION_DEDE, _T("&German (de-de)"));
    ::AppendMenu(localization_menu, MF_STRING, ADOBE_LOCALIZATION_JAJP, _T("&Japanese (ja-jp)"));
    ::AppendMenu(localization_menu, MF_STRING, ADOBE_LOCALIZATION_KOKR, _T("&Korean (ko-kr)"));
    ::AppendMenu(localization_menu, MF_STRING, ADOBE_LOCALIZATION_PGPG, _T("&Pig Latin (pg-pg)"));

    ::AppendMenu(menu_bar, MF_POPUP, hackery::cast<UINT_PTR>(app_menu), _T("&Adobe Begin"));
    ::AppendMenu(menu_bar, MF_POPUP, hackery::cast<UINT_PTR>(view_options_menu), _T("&View Options"));
    ::AppendMenu(menu_bar, MF_POPUP, hackery::cast<UINT_PTR>(localization_menu), _T("&Localization"));

    ::DrawMenuBar(root_window);

    // set root in display
    adobe::get_main_display().set_root(root_window);
}

/****************************************************************************************************/

namespace hackery {
bool LPCWSTR_to_string(const WCHAR* wide_string, std::string& result)
{
    typedef        std::vector<char> buffer_t;

    // we know any utf8 encoded character can't be more than 6 bytes,
    // so we just allocate an extra-large buffer to begin with instead
    // of calling WideCharToMultiByte twice.

    std::size_t     wide_string_length(std::wcslen(wide_string));
    std::size_t     buffer_size(wide_string_length * 6);
    buffer_t        buffer(buffer_size + 1);

    //
    // Convert characters.
    //

    int result_size(WideCharToMultiByte(CP_UTF8, 0, wide_string, static_cast<int>(wide_string_length),
                                        &buffer[0], static_cast<int>(buffer_size), 0, 0));

    if (result_size == 0)
        return false;

    result.assign(std::string(&buffer[0], &buffer[result_size]));

    return true;
}
} // namespace hackery

/****************************************************************************************************/

bool os_initialize( application_t* app )
{
    //
    // We only need to set the resource path on Windows. There are no
    // system events to install handlers for, etc. We say that the
    // resource path is the folder with the executable inside.
    //

    LPWSTR *szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if( NULL == szArglist )
      throw std::runtime_error("CommandLineToArgvW failed");


    std::string directory_string;

    if (!hackery::LPCWSTR_to_string(szArglist[0], directory_string))
        throw std::runtime_error("Path character conversion failed.");

    std::string file1;

    if (nArgs >= 2 && !hackery::LPCWSTR_to_string(szArglist[1], file1))
        throw std::runtime_error("Path character conversion failed.");

    std::string file2;

    if (nArgs >= 3 && !hackery::LPCWSTR_to_string(szArglist[2], file2))
        throw std::runtime_error("Path character conversion failed.");

    clip_quotes(directory_string);

    //
    // Now we need to get a directory from the command line name.
    //
    boost::filesystem::path directory( directory_string, boost::filesystem::native );

    //
    // Tell the application...
    //
    app->set_resource_directory( directory.branch_path() );

    setup_main_window( app );

    if (!file1.empty())
    {
        clip_quotes(file1);
        open_document(app, file1);
    }

    if (!file2.empty())
    {
        clip_quotes(file2);
        open_document(app, file2);
    }

    LocalFree(szArglist);

    return true;
}

/****************************************************************************************************/

void os_mainloop(adobe::application_t& app)
{
    //
    // This is just the standard Win32 message pump.
    //
    MSG msg;

    while ( GetMessage( &msg, 0, 0, 0 ) )
    {
        try
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        catch ( ... )
        {
            adobe::report_exception();
        }

        adobe::general_deferred_proc_queue()();
    }
}

/****************************************************************************************************/

void os_end_mainloop()
{
    //
    // This is just the standard Win32 quit message.
    //
    ::PostQuitMessage(0);
}

/****************************************************************************************************/

}

/****************************************************************************************************/

int __stdcall WinMain( HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpCmdLine, int nCmdShow )
{
    try
    {
        adobe::application_t* theApp = adobe::application_t::getInstance();

        if( theApp ) theApp->run();
    }
    catch( ... )
    {
        adobe::report_exception();
    }

    return 0;
} 

