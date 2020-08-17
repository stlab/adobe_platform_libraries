/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/future/platform_drag_and_drop_data.hpp>

#include <adobe/name.hpp>

#include <vector>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/


/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

// The type aggregate_name_t is missing in the latest adobe_source_libraries.
// Switch to static_name_t.
//
static_name_t key_drag_ref  = "drag_ref"_name;
static_name_t key_drag_item = "drag_item"_name;
static_name_t key_flavor    = "flavor"_name;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
