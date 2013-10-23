/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
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
