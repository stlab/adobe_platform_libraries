/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_CARBON_SAFE_INCLUDE_HPP
#define ADOBE_MACINTOSH_CARBON_SAFE_INCLUDE_HPP

/****************************************************************************************************/

#ifdef __MWERKS__
    #pragma warn_implicitconv off 
#endif

#include <Carbon/Carbon.h>
#include <AudioToolbox/AudioServices.h>

#ifdef __MWERKS__
    #pragma warn_implicitconv reset 
#endif

/*
    REVISIT (sparent) : Apple insists that you include the entire Carbon/Carbon.h framework to
    use any of Carbon. This header file #defines check which conflicts with boost. The work-
    around is to undef check here.
    
    (In an unbelieveable set of circumstances, I wrote the check macro!)
*/

#ifdef check
    #undef check
#endif

/****************************************************************************************************/

#endif

/****************************************************************************************************/
