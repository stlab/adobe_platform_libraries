/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/macintosh_events.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/istream_fwd.hpp>
#include <adobe/istream.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

pascal ::OSStatus do_handle_event(::EventHandlerCallRef call_ref,
                                  ::EventRef            event,
                                  void*                 data)
try
{
    adobe::event_handler_t& myself(*reinterpret_cast<adobe::event_handler_t*>(data));

    return myself.monitor_proc_m.empty() ?
               eventNotHandledErr :
               myself.monitor_proc_m(call_ref, event);
}
catch (const adobe::stream_error_t& err)
{
    adobe::report_error(format_stream_error(err));
    return eventNotHandledErr;
}
catch (const std::exception& err)
{
    adobe::report_error(std::string("Exception: ") + err.what());
    return eventNotHandledErr;
}
catch (...)
{
    adobe::report_error("Exception: Unknown");
    return eventNotHandledErr;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

event_handler_t::event_handler_t() :
    upp_m(::NewEventHandlerUPP(do_handle_event))
{ }

/**************************************************************************************************/

void event_handler_t::install(::EventTargetRef target)
{
    if (is_installed())
        throw std::runtime_error("Event handler already bound to a target.");

    ::EventHandlerRef handler_ref(0);

    ::ADOBE_REQUIRE_STATUS(::InstallEventHandler(target,
                                                 upp_m.get(),
                                                 event_type_set_m.size(),
                                                 &event_type_set_m[0],
                                                 this,
                                                 &handler_ref));

    ref_m.reset(handler_ref);
}

/**************************************************************************************************/

void event_handler_t::uninstall()
{
    if (!is_installed())
        return;

    ref_m.reset(0);
    upp_m.reset(0);
}

/**************************************************************************************************/

void event_handler_t::insert(const ::EventTypeSpec& event)
{
    event_type_set_t::iterator pos(adobe::find(event_type_set_m, event));

    if (pos != event_type_set_m.end())
        return;

    event_type_set_m.push_back(event);

    if (!is_installed())
        return;

    ::ADOBE_REQUIRE_STATUS(::AddEventTypesToHandler(ref_m.get(), 1, &event));
}

/**************************************************************************************************/

void event_handler_t::erase(const ::EventTypeSpec& event)
{
    event_type_set_t::iterator pos(find(event_type_set_m, event));

    if (pos == event_type_set_m.end())
        return;

    event_type_set_m.erase(pos);

    if (!is_installed())
        return;

    ::ADOBE_REQUIRE_STATUS(::RemoveEventTypesFromHandler(ref_m.get(), 1, &event));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
