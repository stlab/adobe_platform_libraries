/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WINCAST_HPP
#define ADOBE_WINCAST_HPP

#include <adobe/config.hpp>

/****************************************************************************************************/

namespace hackery {

/****************************************************************************************************/

template <typename T, typename U>
inline T cast(U u)
{
    // REVISIT (fbrereto) : This function circumvents an issue where MSVC won't allow for the casting
    //                      of a primitive of smaller type to one of larger type. Please use sparingly,
    //                      but when this type of cast must be made use this function so it can be
    //                      pulled out easily.

// because it might be cygwin
#if ADOBE_PLATFORM_WIN
    #pragma warning ( push )
    #pragma warning ( disable : 4311 ) // typecast pointer truncation from type to type
    #pragma warning ( disable : 4312 ) // typecast conversion from type to type of greater size
    #pragma warning ( disable : 4800 ) // conversion to true/false performance warning
#endif

    return (T)(u);

// because it might be cygwin
#if ADOBE_PLATFORM_WIN
    #pragma warning ( pop )
#endif
}

/****************************************************************************************************/
    
} // namespace hackery

/****************************************************************************************************/

#endif

/****************************************************************************************************/
