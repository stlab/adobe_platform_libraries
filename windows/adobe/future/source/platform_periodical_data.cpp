/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/platform_periodical_data.hpp>

#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::pair<UINT_PTR, adobe::periodical_platform_data_t*> timer_set_pair_t;
typedef std::vector<timer_set_pair_t>                           timer_set_t;

/****************************************************************************************************/

// REVISIT (fbrereto) : If there is a better way to get the information into the 

timer_set_t& timer_set()
{
    static timer_set_t confounding_win32_hack_s;

    return confounding_win32_hack_s;
}

/****************************************************************************************************/

timer_set_t::iterator find_timer_for(UINT_PTR index)
{
    timer_set_t&          set(timer_set());
    timer_set_t::iterator first(set.begin());
    timer_set_t::iterator last(set.end());

    for (; first != last; ++first)
    {
        if (first->first == index)
            break;
    }

    return first;
}

/****************************************************************************************************/

void periodical_timer_callback(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR timer_id, DWORD /*dwTime*/)
{
    timer_set_t::iterator iter(find_timer_for(timer_id));

    if (iter == timer_set().end())
        return;

    adobe::periodical_platform_data_t& impl(*(iter->second));

    assert(impl.fire_m);

    impl.fire_m();
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
    timer_ref_m = ::SetTimer(NULL,
                             0,
                             static_cast<UINT>(millisecond_delay),
                             (TIMERPROC)&periodical_timer_callback);

    timer_set().push_back(timer_set_pair_t(timer_ref_m, this));
}

/****************************************************************************************************/

periodical_platform_data_t::~periodical_platform_data_t()
{
    timer_set_t::iterator iter(find_timer_for(timer_ref_m));

    if (iter != timer_set().end())
        timer_set().erase(iter);

    ::KillTimer(NULL, timer_ref_m);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
