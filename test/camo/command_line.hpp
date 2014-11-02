/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <vector>
#include <string>
#include <utility>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

typedef std::pair<int, std::string> task_result_t;
typedef std::vector<std::string>    arg_set_t;

/*************************************************************************************************/

task_result_t command_line(const char* command, const arg_set_t& arg_set);

/*************************************************************************************************/

inline task_result_t command_line(const char* command, const std::string& param)
{
    return command_line(command, arg_set_t(1, param));
}

/*************************************************************************************************/

template <typename ForwardIterator>
inline task_result_t command_line(const char* command, ForwardIterator first, ForwardIterator last)
{
    return command_line(command, arg_set_t(first, last));
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
