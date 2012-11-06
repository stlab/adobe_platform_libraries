/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1

#include <windows.h>

#include <adobe/future/cursor.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/resources.hpp>
#include <adobe/future/source/cursor_stack.hpp>
#include <adobe/future/windows_graphic_utils.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/memory.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/rgba.hpp>

/****************************************************************************************************/

adobe_cursor_t make_cursor(const char* cursor_path, float hot_spot_x, float hot_spot_y)
{
    boost::gil::rgba8_image_t cursor_image;

    adobe::image_slurp(boost::filesystem::path(cursor_path, boost::filesystem::native), cursor_image);

    HBITMAP cursor_bitmap(adobe::to_bitmap(cursor_image));
    HCURSOR result(adobe::to_cursor(cursor_bitmap, static_cast<int>(hot_spot_x), static_cast<int>(hot_spot_y)));

    ::DeleteObject(cursor_bitmap);
    
    return hackery::cast<adobe_cursor_t>(result);
}

/****************************************************************************************************/

void push_cursor(adobe_cursor_t cursor)
{
    cursor_stack_push(cursor);

    ::SetCursor(hackery::cast<HCURSOR>(cursor));
}

/****************************************************************************************************/

adobe_cursor_t pop_cursor()
{
    adobe_cursor_t old_cursor = cursor_stack_top();

    cursor_stack_pop();

    adobe_cursor_t new_cursor = cursor_stack_top();

    if (new_cursor)
    {
        ::SetCursor(hackery::cast<HCURSOR>(new_cursor));
    }
    else
    {
        ::SetCursor(hackery::cast<HCURSOR>(IDC_ARROW));
    }

    return old_cursor;
}

/****************************************************************************************************/

void reset_cursor()
{
    cursor_stack_reset();

    ::SetCursor(hackery::cast<HCURSOR>(IDC_ARROW));
}

/****************************************************************************************************/

void delete_cursor(adobe_cursor_t cursor)
{
    ::DestroyCursor(hackery::cast<HCURSOR>(cursor));
}

/****************************************************************************************************/
