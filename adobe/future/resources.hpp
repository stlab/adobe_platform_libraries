/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIDGET_RESOURCES_HPP
#define ADOBE_WIDGET_RESOURCES_HPP

/****************************************************************************************************/

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void push_resource_root_path(const boost::filesystem::path& root);
void pop_resource_root_path();

boost::filesystem::path find_resource(const boost::filesystem::path& name);

/****************************************************************************************************/

struct resource_context_t
{
    explicit resource_context_t(const boost::filesystem::path& root)
    {
        push_resource_root_path(root);
    }

    ~resource_context_t()
    {
        pop_resource_root_path();
    }
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_WIDGET_RESOURCES_HPP
#endif

/****************************************************************************************************/
