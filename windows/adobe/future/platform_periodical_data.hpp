/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_PERIODICAL_IMPL_HPP
#define ADOBE_PERIODICAL_IMPL_HPP

/****************************************************************************************************/

#define WINDOWS_LEAN_AND_MEAN 1

#include <windows.h>

#include <boost/function.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct periodical_platform_data_t
{
    typedef boost::function<void ()> fire_proc_t;

    periodical_platform_data_t(const fire_proc_t& fire_proc, std::size_t millisecond_delay);

    ~periodical_platform_data_t();

    UINT_PTR    timer_ref_m;
    fire_proc_t fire_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_PERIODICAL_IMPL_HPP
#endif

/****************************************************************************************************/
