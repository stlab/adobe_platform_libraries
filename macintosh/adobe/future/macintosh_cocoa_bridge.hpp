/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_CARBON_COCOA_BRIDGE_HPP
#define ADOBE_CARBON_COCOA_BRIDGE_HPP

/****************************************************************************************************/

#include <Cocoa/Cocoa.h>

/****************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************/

// You must call this routine before you make any Cocoa calls from your Carbon app.
// You may safely call this routine more than once.

void carbon_cocoa_bridge_initialize();

/****************************************************************************************************/

#ifdef __cplusplus
} // extern "C"
#endif

/****************************************************************************************************/
#ifdef __cplusplus
namespace adobe {

/****************************************************************************************************/

class objc_auto_release_pool_t
{
public:
    explicit objc_auto_release_pool_t(bool initialize_bridge = true);

    ~objc_auto_release_pool_t();

private:
    objc_auto_release_pool_t(const objc_auto_release_pool_t& rhs);
    objc_auto_release_pool_t& operator=(const objc_auto_release_pool_t& rhs);

    ::NSAutoreleasePool* pool_m;
};

/****************************************************************************************************/

} // namespace adobe
#endif
/****************************************************************************************************/

// ADOBE_CARBON_COCOA_BRIDGE_HPP
#endif

/****************************************************************************************************/
