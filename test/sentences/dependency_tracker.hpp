/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_DEPENDENCY_TRACKER_HPP
#define ADOBE_SENTENCES_DEPENDENCY_TRACKER_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <vector>

#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/unique.hpp>

/**************************************************************************************************/

typedef void*                        dependency_type; // better type for this purpose?
typedef std::vector<dependency_type> dependency_set_t;

/**************************************************************************************************/

struct dependency_tracker_t
{
    void insert(dependency_type dependency)
    { dependency_set_m.push_back(dependency); }

    dependency_set_t get()
    {
        adobe::sort(dependency_set_m);

        dependency_set_m.erase(adobe::unique(dependency_set_m), dependency_set_m.end());

        return dependency_set_m;
    }

private:
    dependency_set_t dependency_set_m;
};

/**************************************************************************************************/
// ADOBE_SENTENCES_DEPENDENCY_TRACKER_HPP
#endif

/**************************************************************************************************/
