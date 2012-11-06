/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_UI_CORE_FOCUS_HANDLER_HPP
#define ADOBE_UI_CORE_FOCUS_HANDLER_HPP

/**************************************************************************************************/

// REVISIT (sparent) : I believe anything in here should be in an implementation namespace.

#if !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <adobe/future/widgets/headers/widget_utils.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct focus_handler_t
{
    focus_handler_t()
    {
        event_handler_m.insert(kEventClassControl, kEventControlSetFocusPart);

        event_handler_m.monitor_proc_m = boost::bind(&focus_handler_t::handle_event,
                                                     boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(    ::EventHandlerCallRef   next,
                                ::EventRef              event)
    {
        ::OSStatus result(::CallNextEventHandler(next, event)); // Handle us last

        if (user_callback_m.empty())
            return result;

        ::ControlPartCode part(kControlFocusNoPart);

        get_event_parameter<kEventParamControlPart>(event, part);

        user_callback_m(part != kControlFocusNoPart);

        return result;
    }

    boost::function<void (bool)> user_callback_m;
    event_handler_t              event_handler_m;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
// !defined(ADOBE_NO_DOCUMENTATION)

#endif
// ADOBE_UI_CORE_FOCUS_HANDLER_HPP

/**************************************************************************************************/
