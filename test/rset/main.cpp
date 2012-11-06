#include <adobe/future/rset.hpp>

#include <adobe/name.hpp>

#include <iostream>
#include <cassert>

int main()
{
    typedef adobe::rset<adobe::name_t, std::string> set_type;

    set_type set;

    set.push_back(adobe::static_name_t("first_name"),  "first_string");
    set.push_back(adobe::static_name_t("second_name"), "second_string");
    set.push_back(adobe::static_name_t("third_name"),  "third_string");
    set.push_back(adobe::static_name_t("fourth_name"), "fourth_string");
    set.push_back(adobe::static_name_t("fifth_name"),  "fifth_string");

    adobe::name_t find1_result;
    std::string   find2_result;

    find1_result = set.find1(std::string("second_string"));
    find2_result = set.find2(adobe::static_name_t("third_name"));

    assert(find1_result == adobe::static_name_t("second_name"));
    assert(find2_result == std::string("third_string"));

    find1_result = set[std::string("second_string")];
    find2_result = set[adobe::static_name_t("third_name")];

    assert(find1_result == adobe::static_name_t("second_name"));
    assert(find2_result == std::string("third_string"));

    find1_result = const_cast<const set_type&>(set)[std::string("second_string")];
    find2_result = const_cast<const set_type&>(set)[adobe::static_name_t("third_name")];

    assert(find1_result == adobe::static_name_t("second_name"));
    assert(find2_result == std::string("third_string"));

    try
    {
        set[adobe::static_name_t("not_here!")];

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

    set2[std::string("second_string")] = adobe::static_name_t("foo");

    assert (set != set2);

    return 0;
}
