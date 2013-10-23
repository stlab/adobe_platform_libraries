/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_CARBON_MOUSE_WHEEL_HANDLER_HPP
#define ADOBE_CARBON_MOUSE_WHEEL_HANDLER_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/future/macintosh_events.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

typedef boost::function<void (long, bool)> mouse_wheel_handler_proc_t;

/*************************************************************************************************/

struct mouse_wheel_handler_t
{
    mouse_wheel_handler_t()
    {
        // REVISIT (fbrereto) : When the minimum supported OS is 10.4, we should hook
        //                      kEventClassMouse / kEventMouseScroll instead.

        handler_m.insert(kEventClassMouse, kEventMouseWheelMoved);
    }

    ::OSStatus handle_event(::EventHandlerCallRef /*next*/,
                            ::EventRef            event)
    {
        if (!callback_m)
            return noErr;

        ::EventMouseWheelAxis axis;
        ::UInt32              modifiers;
        ::SInt32              delta;

        get_event_parameter<kEventParamMouseWheelAxis>(event, axis);
        get_event_parameter<kEventParamMouseWheelDelta>(event, delta);
        get_event_parameter<kEventParamKeyModifiers>(event, modifiers);

        callback_m(delta, (modifiers & optionKey) != 0);

        return noErr;
    }

    event_handler_t            handler_m;
    mouse_wheel_handler_proc_t callback_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_CARBON_MOUSE_WHEEL_HANDLER_HPP
#endif

/****************************************************************************************************/
