/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_ERROR_HPP
#define ADOBE_MACINTOSH_ERROR_HPP

/****************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/function.hpp>

#include <sstream>
#include <stdexcept>

/****************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

class os_exception : public std::exception
{
public:
    os_exception(long status, const char* file, long line) throw()
    {
        try
        {
            std::stringstream t;
            t << status;
            format(t.str().c_str(), file, line);
        }
        catch (...)
        { }
    }

    os_exception(const char* status, const char* file, long line) throw()
    {
        format(status, file, line);
    }

    os_exception(const std::string& status, const char* file, long line) throw()
    {
        format(status.c_str(), file, line);
    }

    ~os_exception() throw() {}

    const char* what () const throw()
    { return what_m.c_str(); }

private:

    void format(const char* status, const char* file, long line)
    {
        try
        {
            std::stringstream t;
            t << "Error: " << status << " (" << file << ", line " << line << ")";
            what_m.assign(t.str());
        }
        catch (...)
        { }
    }

    std::string what_m;
};

/****************************************************************************************************/

typedef boost::function<void (std::string)> error_handler_proc_t;

//
/// \ingroup apl_widgets
///
/// Use this to specify the callback used in the case when a UI element
/// returns an error of some kind. Note that this is only a service provided
/// to the mac community because clients are unable to throw across the
/// event loop. Our typical error handling strategy should be throw/catch,
/// but the Mac is limited in this regard so we provide this alternative.
//

void set_error_handler(const error_handler_proc_t& proc);

//
/// \ingroup apl_widgets
///
/// Used to report an error through the ui_core_error_handler proc
//

void report_error(const std::string& error);

/****************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

inline void ADOBE_REQUIRE_STATUS_impl(long status, const char* file, long line)
{
//    if (status != 0)
//        ::AudioServicesPlayAlertSound(kUserPreferredAlert);

    // REVISIT (fbrereto) : Turn this back on when we have an interface to handle
    //                      errors on the callback side of the OS event loop
    return;

    if (status != 0)
        throw os_exception(status, file, line);
}

/*************************************************************************************************/

#define ADOBE_REQUIRE_STATUS(x) adobe::implementation::ADOBE_REQUIRE_STATUS_impl((x), __FILE__, __LINE__)

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
