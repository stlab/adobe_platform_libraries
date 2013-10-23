/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_COMMAND_SYSTEM_FWD_HPP
#define ADOBE_SENTENCES_COMMAND_SYSTEM_FWD_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/future/enum_ops.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

enum event_t
{
    disable_k = 1 << 0UL,
    enable_k  = 1 << 1UL,
    add_k     = 1 << 2UL,
    remove_k  = 1 << 3UL
};

ADOBE_DEFINE_BITSET_OPS(event_t)

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_COMMAND_SYSTEM_FWD_HPP
#endif

/**************************************************************************************************/
