/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_MACINTOSH_EVENTS_HPP
#define ADOBE_MACINTOSH_EVENTS_HPP

/**************************************************************************************************/

#if __LP64__
    #error "Carbon is not supported in 64 bit environments."
#else

/**************************************************************************************************/

#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/unique.hpp>
#include <adobe/future/macintosh_error.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <iterator>
#include <vector>

/**************************************************************************************************/

inline bool operator == (const ::EventTypeSpec& x, const ::EventTypeSpec& y)
{ return x.eventClass == y.eventClass && x.eventKind == y.eventKind; }

inline bool operator < (const ::EventTypeSpec& x, const ::EventTypeSpec& y)
{ return x.eventClass < y.eventClass || (x.eventClass == y.eventClass && x.eventKind < y.eventKind); }

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename T> struct event_param_type;

#define ADOBE_EVENT_PARAM_TYPE_HELPER(type, event_type) \
template <> struct event_param_type< type > { static const ::EventParamType value = event_type; }

ADOBE_EVENT_PARAM_TYPE_HELPER(::CFArrayRef,             typeCFArrayRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFDictionaryRef,        typeCFDictionaryRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFMutableArrayRef,      typeCFMutableArrayRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFMutableDictionaryRef, typeCFMutableDictionaryRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFMutableStringRef,     typeCFMutableStringRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFStringRef,            typeCFStringRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::CFTypeRef,              typeCFTypeRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::ControlPartCode,        typeControlPartCode);
ADOBE_EVENT_PARAM_TYPE_HELPER(::DragRef,                typeDragRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::EventMouseWheelAxis,    typeMouseWheelAxis);
ADOBE_EVENT_PARAM_TYPE_HELPER(::EventTargetRef,         typeEventTargetRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(::HICommand,              typeHICommand);
ADOBE_EVENT_PARAM_TYPE_HELPER(::HIPoint,                typeHIPoint);
ADOBE_EVENT_PARAM_TYPE_HELPER(::SInt32,                 typeLongInteger);
ADOBE_EVENT_PARAM_TYPE_HELPER(::TabletPointRec,         typeTabletPointRec);
ADOBE_EVENT_PARAM_TYPE_HELPER(::TabletProximityRec,     typeTabletProximityRec);
ADOBE_EVENT_PARAM_TYPE_HELPER(::UInt32,                 typeUInt32);
ADOBE_EVENT_PARAM_TYPE_HELPER(::WindowRef,              typeWindowRef);
ADOBE_EVENT_PARAM_TYPE_HELPER(bool,                     typeBoolean);

#undef ADOBE_EVENT_PARAM_TYPE_HELPER

/**************************************************************************************************/

template < ::EventParamName name, typename T >
bool get_event_parameter(::EventRef       the_event,
                         T&               value,
                         ::EventParamType type = event_param_type<T>::value)
{
    ::EventParamType actual_type;
    ::UInt32         actual_size;
    ::OSStatus       err(::GetEventParameter(the_event,
                                             name,
                                             type,
                                             &actual_type,
                                             sizeof(T),
                                             &actual_size,
                                             &value));

    return err == noErr && actual_type == type && actual_size == sizeof(T);
}

/**************************************************************************************************/

template < ::EventParamName name, typename T >
bool set_event_parameter(::EventRef       the_event,
                         const T&         value,
                         ::EventParamType type = event_param_type<T>::value)
{
    ::OSStatus err(::SetEventParameter(the_event,
                                       name,
                                       type,
                                       sizeof(T),
                                       &value));

    return err == noErr;
}

/**************************************************************************************************/

template <typename Target>
::EventTargetRef get_event_target(Target target);

#define ADOBE_GET_EVENT_TARGET_HELPER(type, proc) \
template <> inline ::EventTargetRef get_event_target<>(type target) { return proc(target); }

ADOBE_GET_EVENT_TARGET_HELPER(::WindowRef,      ::GetWindowEventTarget)
ADOBE_GET_EVENT_TARGET_HELPER(::ControlRef,     ::GetControlEventTarget)
ADOBE_GET_EVENT_TARGET_HELPER(::MenuRef,        ::GetMenuEventTarget)
ADOBE_GET_EVENT_TARGET_HELPER(::EventTargetRef, ::EventTargetRef)

#undef ADOBE_GET_EVENT_TARGET_HELPER

/**************************************************************************************************/

class event_handler_t : boost::noncopyable
{
public:
    typedef boost::function< ::OSStatus (::EventHandlerCallRef, ::EventRef) > monitor_proc_t;

    event_handler_t();

    template <typename Target>
    void install(Target target)
        { install(get_event_target(target)); }

    void install(::EventTargetRef target);

    void uninstall();

    bool is_installed() const
        { return ref_m.get() != 0; }

    void insert(const ::EventTypeSpec& event_spec);

    void insert(::UInt32 event_class, ::UInt32 event_kind)
        { ::EventTypeSpec event = { event_class, event_kind }; insert(event); }

    template <typename ForwardIterator>
    void insert(const ForwardIterator first, const ForwardIterator last)
    {
        typedef typename std::iterator_traits<ForwardIterator>::value_type        value_type;
        typedef typename std::iterator_traits<ForwardIterator>::iterator_category iterator_category;

        BOOST_STATIC_ASSERT((boost::is_convertible<value_type, ::EventTypeSpec>::value));

        insert(first, last, iterator_category());
    }

    void erase(const ::EventTypeSpec& event_spec);

    void erase(::UInt32 event_class, ::UInt32 event_kind)
        { ::EventTypeSpec event = { event_class, event_kind }; erase(event); }

    template <typename ForwardIterator>
    void erase(const ForwardIterator first, const ForwardIterator last)
    {
        typedef typename std::iterator_traits<ForwardIterator>::value_type        value_type;
        typedef typename std::iterator_traits<ForwardIterator>::iterator_category iterator_category;

        BOOST_STATIC_ASSERT((boost::is_convertible<value_type, ::EventTypeSpec>::value));

        erase(first, last, iterator_category());
    }

    monitor_proc_t monitor_proc_m; // no need to be private

private:
    template <typename ForwardIterator>
    void insert(const ForwardIterator first,
                const ForwardIterator last,
                std::forward_iterator_tag);

    template <typename ForwardIterator>
    void insert(const ForwardIterator first,
                const ForwardIterator last,
                std::random_access_iterator_tag);

    template <typename ForwardIterator>
    void erase(const ForwardIterator first,
               const ForwardIterator last,
               std::forward_iterator_tag);

    template <typename ForwardIterator>
    void erase(const ForwardIterator first,
               const ForwardIterator last,
               std::random_access_iterator_tag);

    typedef adobe::auto_resource< ::EventHandlerRef > auto_ref_t;
    typedef adobe::auto_resource< ::EventHandlerUPP > auto_upp_t;
    typedef std::vector< ::EventTypeSpec >            event_type_set_t;

    auto_ref_t       ref_m;
    auto_upp_t       upp_m;
    event_type_set_t event_type_set_m;
};

/**************************************************************************************************/

template <typename ForwardIterator>
void event_handler_t::insert(const ForwardIterator first,
                             const ForwardIterator last,
                             std::forward_iterator_tag)
{
    typedef void (event_handler_t::*proc_type)(const ::EventTypeSpec&);

    proc_type proc(&event_handler_t::insert);

    std::for_each(first, last, boost::bind(proc, boost::ref(*this), _1));
}

/**************************************************************************************************/

template <typename ForwardIterator>
void event_handler_t::insert(const ForwardIterator first,
                             const ForwardIterator last,
                             std::random_access_iterator_tag)
{
    std::size_t n(std::distance(first, last));

    // this will fill the event_type_set_m vector with new
    // events without duplicating ones already present.
    event_type_set_m.insert(event_type_set_m.end(), first, last);
    adobe::sort(event_type_set_m);
    event_type_set_m.erase(adobe::unique(event_type_set_m), event_type_set_m.end());

    if (!is_installed())
        return;

    // now add the event set to the handler.
    ::ADOBE_REQUIRE_STATUS(::AddEventTypesToHandler(ref_m.get(), n, first));
}

/**************************************************************************************************/

template <typename ForwardIterator>
void event_handler_t::erase(const ForwardIterator first,
                            const ForwardIterator last,
                            std::forward_iterator_tag)
{
    typedef void (event_handler_t::*proc_type)(const ::EventTypeSpec&);

    proc_type proc(&event_handler_t::erase);

    std::for_each(first, last, boost::bind(proc, boost::ref(*this), _1));
}

/**************************************************************************************************/

template <typename ForwardIterator>
void event_handler_t::erase(const ForwardIterator first,
                            const ForwardIterator last,
                            std::random_access_iterator_tag)
{
    std::size_t n(std::distance(first, last));

    adobe::sort(event_type_set_m);

    for (ForwardIterator iter(first); iter != last; ++iter)
    {
        ForwardIterator found(adobe::lower_bound(event_type_set_m, *iter));

        if (found != event_type_set_m.end() && *found == *iter)
            event_type_set_m.erase(found);
    }

    if (!is_installed())
        return;

    // now remove the event set from the handler.
    ::ADOBE_REQUIRE_STATUS(::RemoveEventTypesFromHandler(ref_m.get(), n, first));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/

#endif

/**************************************************************************************************/
