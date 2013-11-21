/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#include <adobe/future/resources.hpp>

#include <adobe/string.hpp>

#include <boost/filesystem/operations.hpp>

#include <stdexcept>
#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::vector<boost::filesystem::path> stack_type;

/****************************************************************************************************/

stack_type& root_path()
{
    static stack_type root_s;

    return root_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void push_resource_root_path(const boost::filesystem::path& root)
{
    root_path().push_back(root);
}

/****************************************************************************************************/

void pop_resource_root_path()
{
    root_path().pop_back();
}

/****************************************************************************************************/

boost::filesystem::path find_resource(const boost::filesystem::path& name)
{
    stack_type&                  stack(root_path());
    stack_type::reverse_iterator iter(stack.rbegin());
    stack_type::reverse_iterator last(stack.rend());

    for (; iter != last; ++iter)
    {
        boost::filesystem::path candidate(*iter / name);

        if (boost::filesystem::exists(candidate))
            return candidate;
    }

    throw std::runtime_error(std::string() + "Could not locate resource \"" + name.string()
            + "\" in any resource path.");
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
