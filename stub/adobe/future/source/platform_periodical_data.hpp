/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_PERIODICAL_IMPL_HPP
#define ADOBE_PERIODICAL_IMPL_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct periodical_platform_data_t
{
    typedef boost::function<void ()> fire_proc_t;

    periodical_platform_data_t(const fire_proc_t& fire_proc, std::size_t millisecond_delay) :
        fire_m(fire_proc), delay_m(millisecond_delay)
    { }

    ~periodical_platform_data_t();

    fire_proc_t fire_m;
    std::size_t delay_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_PERIODICAL_IMPL_HPP
#endif

/****************************************************************************************************/
