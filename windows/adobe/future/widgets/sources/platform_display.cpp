/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <windows.h>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <cassert>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

display_t& get_main_display()
{
    static display_t display_s;

    return display_s;
}

/****************************************************************************************************/

template <typename DisplayElement>
platform_display_type insert(display_t& display, platform_display_type& position, DisplayElement& element)
{ return display.insert(position, element); }

/****************************************************************************************************/

#if 0
    #pragma mark -
#endif

/****************************************************************************************************/

template <>
platform_display_type display_t::insert<platform_display_type>(platform_display_type& parent, const platform_display_type& element)
{
    static const platform_display_type null_parent_s = platform_display_type();

    if (parent != null_parent_s && parent != get_main_display().root())
    {
        if (::SetWindowPos(element, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE ) == 0)
	        ADOBE_THROW_LAST_ERROR;

        ::SendMessage(parent,
				      WM_CHANGEUISTATE,
				      UISF_HIDEACCEL| UISF_HIDEFOCUS | UIS_INITIALIZE,
				      0);  

        assert(::GetParent(element) == parent);
    }

    return element;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
