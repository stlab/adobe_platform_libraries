/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_CAST_HPP
#define ADOBE_MACINTOSH_CAST_HPP

/****************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

typedef ::CFTypeID (*get_cftypeid_proc_t)();

template <typename ToType> get_cftypeid_proc_t cftypeid_proc_type();

template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFAllocatorRef >()          { return ::CFAllocatorGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFArrayRef >()              { return ::CFArrayGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFAttributedStringRef >()   { return ::CFAttributedStringGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFBagRef >()                { return ::CFBagGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFBinaryHeapRef >()         { return ::CFBinaryHeapGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFBitVectorRef >()          { return ::CFBitVectorGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFBooleanRef >()            { return ::CFBooleanGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFBundleRef >()             { return ::CFBundleGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFCalendarRef >()           { return ::CFCalendarGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFCharacterSetRef >()       { return ::CFCharacterSetGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFDataRef >()               { return ::CFDataGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFDateFormatterRef >()      { return ::CFDateFormatterGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFDateRef >()               { return ::CFDateGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFDictionaryRef >()         { return ::CFDictionaryGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFLocaleRef >()             { return ::CFLocaleGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFMachPortRef >()           { return ::CFMachPortGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFMessagePortRef >()        { return ::CFMessagePortGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFNotificationCenterRef >() { return ::CFNotificationCenterGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFNullRef >()               { return ::CFNullGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFNumberFormatterRef >()    { return ::CFNumberFormatterGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFNumberRef >()             { return ::CFNumberGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFPlugInInstanceRef >()     { return ::CFPlugInInstanceGetTypeID; }
//template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFPlugInRef >()             { return ::CFPlugInGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFReadStreamRef >()         { return ::CFReadStreamGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFRunLoopObserverRef >()    { return ::CFRunLoopObserverGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFRunLoopRef >()            { return ::CFRunLoopGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFRunLoopSourceRef >()      { return ::CFRunLoopSourceGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFRunLoopTimerRef >()       { return ::CFRunLoopTimerGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFSetRef >()                { return ::CFSetGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFSocketRef >()             { return ::CFSocketGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFStringRef >()             { return ::CFStringGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFTimeZoneRef >()           { return ::CFTimeZoneGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFTreeRef >()               { return ::CFTreeGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFURLRef >()                { return ::CFURLGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFUserNotificationRef >()   { return ::CFUserNotificationGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFUUIDRef >()               { return ::CFUUIDGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFWriteStreamRef >()        { return ::CFWriteStreamGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFXMLNodeRef >()            { return ::CFXMLNodeGetTypeID; }
template <> inline get_cftypeid_proc_t cftypeid_proc_type< ::CFXMLParserRef >()          { return ::CFXMLParserGetTypeID; }

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

/*!
    Apple uses the Core Foundation type CFTypeRef for "polymorphic" APIs where one of several
    Core Foundation types may be returned. This utility function will compare the TypeID of
    embedded type with the TypeID of a desired type to which the CFTypeRef should be cast. If
    the TypeIDs match, then the cast is permitted, otherwise the result is a null-pointer.
*/

template <typename R>
inline R cf_cast(::CFTypeRef x)
{
    if (x == 0)
        return reinterpret_cast<R>(0);

    ::CFTypeID src_type_id(::CFGetTypeID(x));
    ::CFTypeID dest_type_id(implementation::cftypeid_proc_type<R>()());

    return reinterpret_cast<R>(src_type_id == dest_type_id ? x : 0);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_MACINTOSH_CAST_HPP
#endif

/****************************************************************************************************/
