/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/macintosh_error.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

void get_control_bounds(::HIViewRef control, ::Rect& bounds, bool absolute)
{
    assert(control);

    static const float  round(.5);
    ::HIRect            cur_bounds = { { 0, 0 }, { 0, 0 } };

    ADOBE_REQUIRE_STATUS(::HIViewGetFrame(control, &cur_bounds));

    if (absolute)
    {
        // bounds are relative to the parent control; this makes them relative to the window.

        ::WindowRef  window(::GetControlOwner(control));
        ::ControlRef root_control(0);

        if (window)
            ::ADOBE_REQUIRE_STATUS(::GetRootControl(window, &root_control));

        if (root_control)
            ::ADOBE_REQUIRE_STATUS(::HIViewConvertRect(&cur_bounds, ::HIViewGetSuperview(control), root_control));
    }

    bounds.left   = static_cast<short>(cur_bounds.origin.x + round);
    bounds.top    = static_cast<short>(cur_bounds.origin.y + round);
    bounds.right  = static_cast<short>(bounds.left + cur_bounds.size.width + round);
    bounds.bottom = static_cast<short>(bounds.top + cur_bounds.size.height + round);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
