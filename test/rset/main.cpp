/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <adobe/future/rset.hpp>

#include <adobe/name.hpp>

#include <iostream>
#include <cassert>

int main()
{
    typedef adobe::rset<adobe::name_t, std::string> set_type;

    set_type set;

    set.push_back(adobe::"first_name"_name,  "first_string");
    set.push_back(adobe::"second_name"_name, "second_string");
    set.push_back(adobe::"third_name"_name,  "third_string");
    set.push_back(adobe::"fourth_name"_name, "fourth_string");
    set.push_back(adobe::"fifth_name"_name,  "fifth_string");

    adobe::name_t find1_result;
    std::string   find2_result;

    find1_result = set.find1(std::string("second_string"));
    find2_result = set.find2(adobe::"third_name"_name);

    assert(find1_result == adobe::"second_name"_name);
    assert(find2_result == std::string("third_string"));

    find1_result = set[std::string("second_string")];
    find2_result = set[adobe::"third_name"_name];

    assert(find1_result == adobe::"second_name"_name);
    assert(find2_result == std::string("third_string"));

    find1_result = const_cast<const set_type&>(set)[std::string("second_string")];
    find2_result = const_cast<const set_type&>(set)[adobe::"third_name"_name];

    assert(find1_result == adobe::"second_name"_name);
    assert(find2_result == std::string("third_string"));

    try
    {
        set[adobe::"not_here!"_name];

        assert(false);
    }
    catch(...)
    { }

    try
    {
        set[std::string("not_here!")];

        assert(false);
    }
    catch(...)
    { }

    set_type set2(set);

    assert (set == set2);

    set2[std::string("second_string")] = adobe::"foo"_name;

    assert (set != set2);

    return 0;
}
