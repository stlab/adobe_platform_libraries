/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ASSEMBLAGE_HPP
#define ADOBE_ASSEMBLAGE_HPP

#include <adobe/config.hpp>

#include <list>

#include <boost/bind/apply.hpp>
#include <boost/function.hpp>
#include <boost/signals2/connection.hpp>

#include <adobe/algorithm/for_each.hpp>
#include <adobe/memory.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

class assemblage_t
{
public:
    ~assemblage_t()
    {
        // disconnect all our connections
        adobe::for_each(cleanup_m, boost::bind(boost::apply<void>(), _1));
    }

    void cleanup(boost::function<void()> f)
    {
        cleanup_m.push_front(f);
    }

private:
    std::list<boost::function<void ()> >          cleanup_m;
};

/****************************************************************************************************/

template <typename T>
inline void assemblage_cleanup_ptr(assemblage_t& assemblage, T* x)
{ assemblage.cleanup(boost::bind(delete_ptr(), x)); }

/****************************************************************************************************/

inline void assemblage_cleanup_connection(assemblage_t& assemblage, boost::signals2::connection& x)
{ assemblage.cleanup(boost::bind(&boost::signals2::connection::disconnect, x)); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
