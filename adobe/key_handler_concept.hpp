/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_KEY_HANDLER_HPP
#define ADOBE_KEY_HANDLER_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/future/platform_primitives.hpp>
#include <adobe/widget_attributes.hpp>

#include <boost/concept_check.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

template <class H> // H models KeyHandler
inline bool handle_key(H& handler, key_type key, bool pressed, modifiers_t modifiers)
{ return handler.handle_key(key, pressed, modifiers); }

/*************************************************************************************************/

template <class H> // H models KeyHandler
inline any_regular_t underlying_handler(H& handler)
{ return handler.underlying_handler(); }

/*************************************************************************************************/

    template <class Handler>
    struct KeyHandlerConcept
    {
        void constraints() {
            handle_key(handler, key, pressed, modifiers);
            underlying_handler(handler);
        }
        Handler handler;
        key_type key;
        bool pressed;
        modifiers_t modifiers;
    };

/*************************************************************************************************/

} //namespace adobe

/*************************************************************************************************/

#endif
