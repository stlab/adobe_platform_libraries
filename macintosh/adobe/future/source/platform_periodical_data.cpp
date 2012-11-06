/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/platform_periodical_data.hpp>

#include <adobe/future/macintosh_error.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

pascal void periodical_timer_callback(::EventLoopTimerRef /*timer*/, void* user_data)
try
{
    adobe::periodical_platform_data_t& impl(*reinterpret_cast<adobe::periodical_platform_data_t*>(user_data));

    assert(impl.fire_m);

    impl.fire_m();
}
catch (const std::exception& err)
{
    adobe::report_error(std::string("Exception: ") + err.what());
}
catch (...)
{
    adobe::report_error("Unknown Error");
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

periodical_platform_data_t::periodical_platform_data_t(const fire_proc_t& fire_proc,
                                                       std::size_t        millisecond_delay) :
    fire_m(fire_proc)
{
    ::EventLoopTimerRef timer_ref;

    timer_upp_ref_m.reset(::NewEventLoopTimerUPP(&periodical_timer_callback));

    if (::InstallEventLoopTimer(::GetCurrentEventLoop(),
                                0.0,
                                millisecond_delay / 1000.0,
                                timer_upp_ref_m.get(),
                                this,
                                &timer_ref) != noErr)
        return;

    timer_ref_m.reset(timer_ref);
}

/****************************************************************************************************/

periodical_platform_data_t::~periodical_platform_data_t()
{ /* the necessary stuff will be auto-destructed */ }

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
