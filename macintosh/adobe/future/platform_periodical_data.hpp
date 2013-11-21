/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_PLATFORM_PERIODICAL_DATA_HPP
#define ADOBE_PLATFORM_PERIODICAL_DATA_HPP

/****************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct periodical_platform_data_t
{
    typedef boost::function<void ()> fire_proc_t;

    periodical_platform_data_t(const fire_proc_t& fire_proc, std::size_t millisecond_delay);

    ~periodical_platform_data_t();

    auto_resource< ::EventLoopTimerUPP > timer_upp_ref_m;
    auto_resource< ::EventLoopTimerRef > timer_ref_m;
    fire_proc_t                          fire_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_PLATFORM_PERIODICAL_DATA_HPP
#endif

/****************************************************************************************************/
