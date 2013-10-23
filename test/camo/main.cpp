/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <iostream>
#include <sstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/regex.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/string.hpp>
#include <adobe/iomanip_pdf.hpp>
#include <adobe/future/modal_dialog_interface.hpp>

#include "command_line.hpp"

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

void trim(std::string& str)
{
    if (str.empty())
        return;

    std::size_t start(0);

    while (start != str.size() && std::isspace(str[start]))
        ++start;

    if (start == str.size())
    {
        str = std::string();

        return;
    }

    std::size_t end(str.size() - 1);

    while (start != end && std::isspace(str[end]))
        --end;

    str = str.substr(start, end - start + 1);
}

/**************************************************************************************************/

int do_command(const char* command, const char** first_arg, const char** last_arg)
{
    // first we get the exact command path. This is to work around an issue where, if the
    // cwd path is "too long", the task will throw an exception ("launch path not accessible")
    // (I think it may have something to do with 'which' returning a string with an LF in it)
    //
    // Unfortunately 'which' always returns 0 so the result cannot be checked for errors.

    adobe::task_result_t which_result(adobe::command_line("/usr/bin/which", command));

    trim(which_result.second);

    adobe::task_result_t cmd_result(adobe::command_line(which_result.second.c_str(), first_arg, last_arg));

    std::cout << cmd_result.second;

    return cmd_result.first;
}

/**************************************************************************************************/

bool always_true(adobe::name_t, const adobe::any_regular_t&)
{
    return true;
}

/**************************************************************************************************/

adobe::any_regular_t cat(const adobe::array_t& arg_set)
{
    std::string result;

    adobe::array_t::const_iterator iter(arg_set.begin());
    adobe::array_t::const_iterator last(arg_set.end());

    for (; iter != last; ++iter)
        if (iter->type_info() == adobe::type_info<adobe::string_t>())
            result << std::string(iter->cast<adobe::string_t>());
        else if (iter->type_info() == adobe::type_info<adobe::name_t>())
            result << std::string(iter->cast<adobe::name_t>().c_str());

    return adobe::any_regular_t(adobe::string_t(result));
}

/**************************************************************************************************/
#if 0
adobe::any_regular_t regexp(const adobe::array_t& arg_set)
{
    if (arg_set.size() < 2)
        throw std::runtime_error("regexp: At least two parameters required.");

    adobe::string_t expression(arg_set[0].cast<adobe::string_t>());
    adobe::string_t haystack(arg_set[1].cast<adobe::string_t>());

    boost::regex  re(expression.c_str(), boost::regex::basic);
    boost::smatch what;

    return adobe::any_regular_t(boost::regex_search(std::string(haystack), what, re));
}
#endif
/**************************************************************************************************/

adobe::any_regular_t string_find(const adobe::array_t& arg_set)
{
    if (arg_set.size() < 2)
        throw std::runtime_error("string_find: At least two parameters required.");

    std::string haystack(arg_set[0].cast<adobe::string_t>());
    std::string needle(arg_set[1].cast<adobe::string_t>());
    long        pos(0);

    if (arg_set.size() > 2)
        arg_set[2].cast(pos);

    return adobe::any_regular_t(haystack.find(needle, pos));
}

/**************************************************************************************************/

adobe::any_regular_t exec(const adobe::array_t& arg_set)
{
    if (arg_set.size() < 1)
        throw std::runtime_error("exec: At least one parameter required.");

    adobe::array_t::const_iterator iter(arg_set.begin());
    adobe::array_t::const_iterator last(arg_set.end());

    const adobe::string_t& command(iter->cast<adobe::string_t>());

    ++iter;

    adobe::arg_set_t arg_string_set;

    for (; iter != last; ++iter)
        arg_string_set.push_back(iter->cast<adobe::string_t>());

    std::string cmd_result;
    
    try
    {
        adobe::task_result_t which_result(adobe::command_line("/usr/bin/which", command));
    
        trim(which_result.second);

        cmd_result = adobe::command_line(which_result.second.c_str(), arg_string_set).second;

        trim(cmd_result);
    }
    catch(...)
    {
        cmd_result = "<exception>";
    }

    // std::cerr << "exec: " << cmd_result << std::endl;

    return adobe::any_regular_t(adobe::string_t(cmd_result));
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main(int argc, char* argv[])
try
{
    if (argc < 2)
    {
        std::cout << "Adobe Camo " << ADOBE_VERSION_MAJOR
                  << '.' << ADOBE_VERSION_MINOR
                  << '.' << ADOBE_VERSION_SUBMINOR
                  << " compiled " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << std::endl;
        std::cout << "Provides UI for UNIX commands via the Adobe Source Libraries." << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << argv[0] << " command" << std::endl;

        return 1;
    }

    boost::filesystem::ifstream adam_stream(boost::filesystem::path(adobe::make_string(argv[1], ".adm"),
                                            boost::filesystem::native));

    if (adam_stream.fail())
        throw std::runtime_error(adobe::make_string("file ", argv[1], ".adm could not be found."));

    boost::filesystem::ifstream eve_stream(boost::filesystem::path(adobe::make_string(argv[1], ".eve"),
                                           boost::filesystem::native));

    if (eve_stream.fail())
        throw std::runtime_error(adobe::make_string("file ", argv[1], ".eve could not be found."));

    adobe::array_t arg_set;

    for(char** first(&argv[1]), **last(&argv[argc]); first != last; ++first)
        arg_set.push_back(adobe::any_regular_t(std::string(*first)));

    adobe::modal_dialog_t dialog;

    dialog.input_m.insert(adobe::dictionary_t::value_type(adobe::static_name_t("argv"), adobe::any_regular_t(arg_set)));
    dialog.input_m.insert(adobe::dictionary_t::value_type(adobe::static_name_t("argc"), adobe::any_regular_t(argc - 1)));
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("cat"), &cat);
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("string_find"), &string_find);
    dialog.vm_lookup_m.insert_array_function(adobe::static_name_t("exec"), &exec);
    dialog.callback_m = &always_true;

#ifndef NDEBUG
    std::cout << adobe::begin_pdf << dialog.input_m << adobe::end_pdf;
#endif

    adobe::dialog_result_t dialog_result(dialog.go(eve_stream, adam_stream) );

    std::cout << adobe::begin_pdf
              << dialog_result.terminating_action_m << std::endl
              << dialog_result.command_m << adobe::end_pdf;

    std::cout << "Success." << std::endl;
#if 0
    return do_command(argv[1], &argv[2], &argv[argc]);
#endif
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return 1;
}
catch(...)
{
    std::cerr << "Exception: unknown" << std::endl;

    return 1;
}

/**************************************************************************************************/
