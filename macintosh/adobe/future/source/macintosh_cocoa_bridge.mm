/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/macintosh_cocoa_bridge.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

#include <cassert>
#include <stdexcept>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

class cocoa_bridge_singleton_t
{
public:
    cocoa_bridge_singleton_t() :
        pool_m(false)
    {
        //  Needed for Carbon based applications which call into Cocoa
        BOOL app_load_ok = ::NSApplicationLoad();

        if (app_load_ok == NO)
            throw std::runtime_error("NSApplicationLoad failed. Cocoa bridge undefined.");

NS_DURING
        // Set up the NSWindow machinery used by some classes (e.g., NSCursor)
        [[[NSWindow alloc] init] release];
NS_HANDLER
        throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
    }

private:
    cocoa_bridge_singleton_t(const cocoa_bridge_singleton_t& rhs);
    cocoa_bridge_singleton_t& operator=(const cocoa_bridge_singleton_t& rhs);

	adobe::objc_auto_release_pool_t pool_m;
};

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************/

void carbon_cocoa_bridge_initialize()
{
    // default construction/static destruction will handle things from here.

    static cocoa_bridge_singleton_t singleton_s;
}

/****************************************************************************************************/

#ifdef __cplusplus
} // extern "C"
#endif

/****************************************************************************************************/
#ifdef __cplusplus
namespace adobe {

/****************************************************************************************************/

objc_auto_release_pool_t::objc_auto_release_pool_t(bool initialize_bridge)
{
    if (initialize_bridge)
        carbon_cocoa_bridge_initialize();

NS_DURING
    pool_m = [[NSAutoreleasePool alloc] init];
NS_HANDLER
    throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
}

objc_auto_release_pool_t::~objc_auto_release_pool_t()
{
NS_DURING
    [pool_m release];
NS_HANDLER
    throw std::runtime_error("Objective-C Exception");
NS_ENDHANDLER
}

/****************************************************************************************************/

} // namespace adobe
#endif
/****************************************************************************************************/
