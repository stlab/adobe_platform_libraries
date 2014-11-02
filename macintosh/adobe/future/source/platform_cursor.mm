/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/cursor.hpp>

#include <adobe/future/image_slurp.hpp>
#include <adobe/future/resources.hpp>
#include <adobe/future/macintosh_cocoa_bridge.hpp>
#include <adobe/future/source/cursor_stack.hpp>
#include <adobe/future/macintosh_graphic_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/gil/image.hpp>

#include <stdexcept>

/****************************************************************************************************/

//  Based off of "Cocoa Drawing Guide: Working with Images"
NSImage* to_NSImage(const char* cursor_path)
{
    boost::gil::rgba8_image_t cursor_image;

    adobe::image_slurp(boost::filesystem::path(cursor_path),
                       cursor_image);

    adobe::auto_resource<adobe::cg_image_t> cursor_cg_image(adobe::make_cg_image(cursor_image));

    CGImageRef cgImageRef = adobe::to_CGImageRef(cursor_cg_image.get());

    if (cgImageRef == NULL)
        return NULL;

    NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);

    // Get the image dimensions.
    imageRect.size.height = CGImageGetHeight(cgImageRef);
    imageRect.size.width = CGImageGetWidth(cgImageRef);

    NSImage* newImage = 0;

NS_DURING
    // Create a new image to receive the Quartz image data.
    newImage = [[[NSImage alloc] initWithSize:imageRect.size] autorelease];
    [newImage lockFocus];

    // Get the Quartz context and draw.
    CGContextRef imageContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage(imageContext, *(CGRect*)&imageRect, cgImageRef);

    [newImage unlockFocus];
NS_HANDLER
    throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER

    return newImage;
}

/****************************************************************************************************/

adobe_cursor_t make_cursor(const char* cursor_path, float hot_spot_x, float hot_spot_y)
{
    adobe::objc_auto_release_pool_t auto_release_pool;

    NSImage*  nsImage = to_NSImage(cursor_path);
    NSCursor* cursor = 0;

NS_DURING
    cursor = [[NSCursor alloc] initWithImage:nsImage hotSpot:NSMakePoint(hot_spot_x, hot_spot_y)];

NS_HANDLER
    throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER

    return (adobe_cursor_t)cursor;
}

/****************************************************************************************************/

void push_cursor(adobe_cursor_t cursor)
{
    cursor_stack_push(cursor);

NS_DURING
    [(NSCursor*)cursor set];
NS_HANDLER
    throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
}

/****************************************************************************************************/

adobe_cursor_t pop_cursor()
{
    adobe_cursor_t old_cursor = cursor_stack_top();

    cursor_stack_pop();

    adobe_cursor_t new_cursor = cursor_stack_top();

    if (new_cursor)
    {
NS_DURING
        [(NSCursor*)new_cursor set];
NS_HANDLER
        throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
    }
    else
    {
        SetThemeCursor(kThemeArrowCursor);
    }

    return old_cursor;
}

/****************************************************************************************************/

void reset_cursor()
{
    cursor_stack_reset();

    SetThemeCursor(kThemeArrowCursor);
}

/****************************************************************************************************/

void delete_cursor(adobe_cursor_t cursor)
{
NS_DURING
	/* cursors can be destroyed at termination time. We must therefore explicitly
	ensure that we have an active auto-release pool */
    adobe::objc_auto_release_pool_t auto_release_pool;

    [(NSCursor*)cursor release];
NS_HANDLER
        throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
}

/****************************************************************************************************/
