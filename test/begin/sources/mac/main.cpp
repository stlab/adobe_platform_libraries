/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/****************************************************************************************************/

#include "express_viewer.hpp"
#include "report_exception.hpp"

#include <boost/filesystem/convenience.hpp>

#include <adobe/xstring.hpp>

#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/future/macintosh_filesystem.hpp>
#include <adobe/future/behavior.hpp>

#include <sstream>
#include <string>

/****************************************************************************************************/

#define kHICommandRefreshView       'Reld'
#define kHICommandRefreshSheet      'RfsS'
#define kHICommandNormalDialogSize  'NrSz'
#define kHICommandSmallDialogSize   'SmSz'
#define kHICommandMiniDialogSize    'MnSz'
#define kHICommandFrameWidgets      'TgFm'
#define kHICommandClearWidgetFrames 'ClFr'
#define kHICommandSerializeWidgets  'SzWg'
#define kHICommandRunModal          'dlog'

#define kHICommandLocalizationENUS  'enus'
#define kHICommandLocalizationDEDE  'dede'
#define kHICommandLocalizationJAJP  'jajp'
#define kHICommandLocalizationKOKR  'kokr'
#define kHICommandLocalizationPGPG  'pgpg'

const CFStringRef kMainNibFileName  (CFSTR( "begin" ));
const CFStringRef kMenuBarNibName   (CFSTR( "MenuBar" ));

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

//
/// Attempt to open the documents in the given AEDescList.
//
void OpenTheDocuments(AEDescList* documents)
{
    adobe::application_t*   theApp = adobe::application_t::getInstance();
    long                        doc_count;
    FSRef                       file;
    AEKeyword                   keyword;
    DescType                    type_code;
    Size                        actual_size;

    ADOBE_REQUIRE_STATUS(AECountItems(documents, &doc_count));
    
    bool is_adam (false);
    bool is_eve (false);

    for (long i = 1 ; i <= doc_count; ++i) 
    {
        /*
        get the i'th FSSpec record.  NOTE: implicity, we are calling
        a coercion handler because this list actually contains alias records. 
        In particular, the coercion handler converts them from alias records
        into FSSpec records.
        */
        ADOBE_REQUIRE_STATUS(AEGetNthPtr(documents, i, typeFSRef, &keyword, &type_code,
            (Ptr) &file, sizeof(file), (actual_size = sizeof(file), &actual_size)));

        //
        // We need to get the directory name and the file name of the document
        // which we have been asked to open.
        //
        bfs::path   file_name( adobe::fsref_to_path( file ) );
        std::string extension( boost::filesystem::extension( file_name ) );

        if (extension == ".eve")
        {
            theApp->set_eve_file( file_name );
            is_eve = true;
        }
        else if (extension == ".adm")
        {
            theApp->set_adam_file( file_name );
            is_adam = true;
        }
        
    }

    if (is_adam) theApp->load_sheet(); // will display window
    else if (is_eve) theApp->display_window();
}

/****************************************************************************************************/

//
/// Handle "open documents" requests coming from AppleEvents.
///
/// \param  appleEvt    the apple event to respond to.
/// \param  reply       the default reply event
/// \param  ref     the AE dispatch table..
//
pascal OSErr handle_open( const AppleEvent* appleEvt, AppleEvent* /*reply*/, long /*ref*/ )
try
{
    OSErr       error;
    AEDescList  documents;

    //
    // Allocate AEDescList's internals.
    //
    AECreateDesc( typeNull, 0, 0, &documents );

    //
    // Try to get the list of documents to open.
    //
    error = AEGetParamDesc( appleEvt, keyDirectObject, typeAEList, &documents );

    if( error == noErr ) OpenTheDocuments( &documents );

    //
    // Release the document list and return.
    //
    AEDisposeDesc( &documents );

    return error;
}
catch( ... )
{
    adobe::report_exception();

    return -1; // REVISIT
}

/****************************************************************************************************/

//
/// Handle commands coming from the menubar.
///
/// \param  handler the next handler to call.
/// \param  event   description of the event.
/// \param  data    the application_t we need to communicate with.
//
static pascal OSStatus menu_command( EventHandlerCallRef /*handler*/, EventRef event, void* data )
try
{
    adobe::application_t*   theApp( static_cast<adobe::application_t*>( data ) );
    OSStatus                result( noErr );
    HICommand               command;

    //
    // We can't do anything without a pointer to the application
    // object.
    //
    if( !theApp ) return eventNotHandledErr;

    if( GetEventParameter( event, kEventParamDirectObject,
        typeHICommand, 0, sizeof( command ), 0, &command ) )
        return eventNotHandledErr;

    //
    // Figure out what we're meant to do.
    //
    switch( command.commandID )
    {
#ifndef NDEBUG
      case kHICommandFrameWidgets:
        theApp->frame_window();
        break;

      case kHICommandClearWidgetFrames:
        theApp->clear_window_frames();
        break;
#else
      case kHICommandFrameWidgets:
      case kHICommandClearWidgetFrames:
        adobe::system_beep();
        break;
#endif

      case kHICommandNormalDialogSize:
        theApp->set_dialog_size( adobe::size_normal_s );
        theApp->display_window();
        break;

      case kHICommandSmallDialogSize:
        theApp->set_dialog_size( adobe::size_small_s );
        theApp->display_window();
        break;

      case kHICommandMiniDialogSize:
        theApp->set_dialog_size( adobe::size_mini_s );
        theApp->display_window();
        break;

      case kHICommandRefreshView:
        theApp->display_window();
        break;

      case kHICommandRefreshSheet:
        theApp->load_sheet();
        break;

      case kHICommandRunModal:
        theApp->run_current_as_modal();
        break;

      case kHICommandLocalizationENUS:
        adobe::implementation::top_frame().attribute_set_m.insert(
            std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("en_US")));
        theApp->display_window();
        break;

      case kHICommandLocalizationDEDE:
        adobe::implementation::top_frame().attribute_set_m.insert(
            std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("de_DE")));
        theApp->display_window();
        break;

      case kHICommandLocalizationJAJP:
        adobe::implementation::top_frame().attribute_set_m.insert(
            std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("ja_JP")));
        theApp->display_window();
        break;

      case kHICommandLocalizationKOKR:
        adobe::implementation::top_frame().attribute_set_m.insert(
            std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("ko_KR")));
        theApp->display_window();
        break;

      case kHICommandLocalizationPGPG:
        adobe::implementation::top_frame().attribute_set_m.insert(
            std::make_pair(adobe::static_token_range("lang"), adobe::static_token_range("pg_PG")));
        theApp->display_window();
        break;

      case kHICommandSave:
      case kHICommandSaveAs:
      case kHICommandClose:
      case kHICommandNew:
      case kHICommandOpen:
      case kHICommandPageSetup:
      case kHICommandPrint:
      case kHICommandSerializeWidgets:
        adobe::system_beep();
        break;

      default:
        result = eventNotHandledErr;
        break;
    }
    return result;
}
catch( ... )
{
    adobe::report_exception();

    return eventNotHandledErr;
}

/****************************************************************************************************/

void do_deferred_dequeue(::EventLoopTimerRef /*inTimer*/,
                         void*               /*data*/)
{
    adobe::general_deferred_proc_queue()();
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

bool os_initialize( adobe::application_t* theApp )
{
    //
    // On the Mac we need to install the application menus, respond
    // to AppleEvents and set the resource path. We set the resource
    // path first.
    //
    ProcessSerialNumber psn;
    ADOBE_REQUIRE_STATUS( GetCurrentProcess( &psn ) );

    FSRef location;
    ADOBE_REQUIRE_STATUS( GetProcessBundleLocation( &psn, &location ) );

    theApp->set_resource_directory( fsref_to_path( location ) / "Contents" / "Resources" );

    //
    // Now load our bundle, sign up for AppleEvents and show the menu.
    //
    CFBundleRef bundle = CFBundleGetMainBundle();
    IBNibRef    nibs = 0;

    if( !bundle ) return false;

    ADOBE_REQUIRE_STATUS( CreateNibReferenceWithCFBundle( bundle, kMainNibFileName, &nibs ) );

    if( !nibs )
    {
        ::CFRelease( bundle );

        return false;
    }

    //
    // Sign up to handle the "Open" AppleEvent.
    //
    static adobe::auto_resource<AEEventHandlerUPP> ae_handler( NewAEEventHandlerUPP( handle_open ) );

    AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments, ae_handler.get(), 0, false );

    //
    // Install the menu, and it's event handler.
    //
    ADOBE_REQUIRE_STATUS( SetMenuBarFromNib( nibs, kMenuBarNibName ) );

    static EventTypeSpec                            hi_event = { kEventClassCommand, kHICommandFromMenu };
    static adobe::auto_resource<EventHandlerUPP>    hi_handler( NewEventHandlerUPP( menu_command ) );

    InstallApplicationEventHandler( hi_handler.get(), 1, &hi_event, theApp, 0 );

    //
    // Register this app as an Appearance Client
    //
    // Apple docs: "This function does nothing on Mac OS X. Do not call it."
    //
    // RegisterAppearanceClient();

    return true;
}

/****************************************************************************************************/

void os_mainloop(adobe::application_t& app)
{
    adobe::auto_resource< ::EventLoopTimerUPP > loop_upp(::NewEventLoopTimerUPP(do_deferred_dequeue));
    adobe::auto_resource< ::EventLoopTimerRef > idle_timer_ref;
    ::EventLoopTimerRef                         temp_timer_ref;

    ADOBE_REQUIRE_STATUS(::InstallEventLoopTimer(::GetMainEventLoop(),
                                                 1,
                                                 .01,
                                                 loop_upp.get(),
                                                 &app,
                                                 &temp_timer_ref));

    idle_timer_ref.reset(temp_timer_ref);

    ::RunApplicationEventLoop();
}

/****************************************************************************************************/

void os_end_mainloop()
{
    ::QuitApplicationEventLoop();
}

/****************************************************************************************************/

}

/****************************************************************************************************/

int main()
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

/****************************************************************************************************/
