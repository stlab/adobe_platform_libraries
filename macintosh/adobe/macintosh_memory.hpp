/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_MEMORY_HPP
#define ADOBE_MACINTOSH_MEMORY_HPP

/****************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/memory.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

#define ADOBE_DELETE_PTR_SPECIALIZATION(type, func) \
template<>                                          \
struct delete_ptr_trait<type>                       \
{                                                   \
    void operator()(type x) const                   \
    { if (x) func(x); }                             \
};

/****************************************************************************************************/

//ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserReceiveDragUPP,              ::DisposeDataBrowserReceiveDragUPP) // same as DataBrowserAcceptDragUPP
//ADOBE_DELETE_PTR_SPECIALIZATION(::HIViewRef,                              ::DisposeControl) // same as ::ControlRef
ADOBE_DELETE_PTR_SPECIALIZATION(::AEEventHandlerUPP,                        ::DisposeAEEventHandlerUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ATSUStyle,                                ::ATSUDisposeStyle)
ADOBE_DELETE_PTR_SPECIALIZATION(::ATSUTextLayout,                           ::ATSUDisposeTextLayout)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFBundleRef,                              ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFLocaleRef,                              ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFMutableDictionaryRef,                   ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFNumberFormatterRef,                     ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFStringRef,                              ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CFURLRef,                                 ::CFRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CGColorSpaceRef,                          ::CGColorSpaceRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CGDataProviderRef,                        ::CGDataProviderRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::CGImageRef,                               ::CGImageRelease)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlEditTextValidationUPP,             ::DisposeControlEditTextValidationUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneActivateUPP,               ::DisposeControlUserPaneActivateUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneDrawUPP,                   ::DisposeControlUserPaneDrawUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneFocusUPP,                  ::DisposeControlUserPaneFocusUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneHitTestUPP,                ::DisposeControlUserPaneHitTestUPP)
//ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneIdleUPP,                   ::DisposeControlUserPaneIdleUPP) // same as ControlEditTextValidationUPP (note: odd!)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneKeyDownUPP,                ::DisposeControlUserPaneKeyDownUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneTrackingUPP,               ::DisposeControlUserPaneTrackingUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserAcceptDragUPP,                 ::DisposeDataBrowserAcceptDragUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserAddDragItemUPP,                ::DisposeDataBrowserAddDragItemUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserDrawItemUPP,                   ::DisposeDataBrowserDrawItemUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserEditItemUPP,                   ::DisposeDataBrowserEditItemUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserGetContextualMenuUPP,          ::DisposeDataBrowserGetContextualMenuUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserHitTestUPP,                    ::DisposeDataBrowserHitTestUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemAcceptDragUPP,             ::DisposeDataBrowserItemAcceptDragUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemCompareUPP,                ::DisposeDataBrowserItemCompareUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemDataUPP,                   ::DisposeDataBrowserItemDataUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemDragRgnUPP,                ::DisposeDataBrowserItemDragRgnUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemHelpContentUPP,            ::DisposeDataBrowserItemHelpContentUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemNotificationUPP,           ::DisposeDataBrowserItemNotificationUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemNotificationWithItemUPP,   ::DisposeDataBrowserItemNotificationWithItemUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemReceiveDragUPP,            ::DisposeDataBrowserItemReceiveDragUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserItemUPP,                       ::DisposeDataBrowserItemUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserPostProcessDragUPP,            ::DisposeDataBrowserPostProcessDragUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserSelectContextualMenuUPP,       ::DisposeDataBrowserSelectContextualMenuUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::DataBrowserTrackingUPP,                   ::DisposeDataBrowserTrackingUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventHandlerRef,                          ::RemoveEventHandler)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventHandlerUPP,                          ::DisposeEventHandlerUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventLoopIdleTimerUPP,                    ::DisposeEventLoopIdleTimerUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventLoopTimerRef,                        ::RemoveEventLoopTimer)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventLoopTimerUPP,                        ::DisposeEventLoopTimerUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::EventRef,									::ReleaseEvent)
ADOBE_DELETE_PTR_SPECIALIZATION(::Handle,                                   ::DisposeHandle)
ADOBE_DELETE_PTR_SPECIALIZATION(::NavDialogRef,                             ::NavDialogDispose)
ADOBE_DELETE_PTR_SPECIALIZATION(::NavReplyRecord*,                          ::NavDisposeReply)

#if !__LP64__
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlUserPaneBackgroundUPP,             ::DisposeControlUserPaneBackgroundUPP)
ADOBE_DELETE_PTR_SPECIALIZATION(::ControlRef,                               ::DisposeControl)
ADOBE_DELETE_PTR_SPECIALIZATION(::MenuRef,                                  ::DisposeMenu)
// ADOBE_DELETE_PTR_SPECIALIZATION(::WindowRef,                                ::ReleaseWindow)
#endif

/****************************************************************************************************/

typedef adobe::auto_resource< ::CFStringRef > auto_cfstring_t;
typedef adobe::auto_resource< ::ControlRef >  auto_control_t;

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
