/*
    Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <iostream>

#include <boost/operators.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

/******************************************************************************/

struct my_custom_type_t : boost::equality_comparable<my_custom_type_t>
{
    my_custom_type_t() : x(42) { }

    int x;
};

inline bool operator==(const my_custom_type_t&, const my_custom_type_t&)
{
    return true;
}

/******************************************************************************/

void test0()
{
    adobe::any_regular_t number(5);
    adobe::any_regular_t text(adobe::string_t("Hello, world!"));
    adobe::any_regular_t boolean(true);
    adobe::any_regular_t identifier(adobe::static_name_t("some_identifier"));

    adobe::dictionary_t  dictionary;
    adobe::array_t       array;
    adobe::string_t      string("Hello, world!");

    dictionary[adobe::static_name_t("key_1")] = number;
    dictionary[adobe::static_name_t("key_2")] = text;

    array.push_back(boolean);
    array.push_back(identifier);

    adobe::any_regular_t* any_regular_pointer(&number);
    adobe::dictionary_t*  dictionary_pointer(&dictionary);
    adobe::array_t*       array_pointer(&array);
    adobe::string_t*      string_pointer(&string);

    /*
        REVISIT (fbrereto): As of 2008/02/20 there is an intermittent bug that crops up
                            when debugging this function. As the routine goes out of
                            scope and the program counter hits the close curly brace,
                            the following crops up in stderr:
                            
                            test_asl_data_formatter(25333) malloc: *** vm_allocate(size=4291837952) failed (error code=3)
                            test_asl_data_formatter(25333) malloc: *** error: can't allocate region
                            test_asl_data_formatter(25333) malloc: *** set a breakpoint in szone_error to debug
                            asl_data_formatter format_any_regular_t exception: St9bad_alloc

                            Like I said, this bug is intermittent. I cannot reproduce
                            it in other routines (even routines identical to this one
                            but called at a later time in the program). I also cannot
                            attach a breakpoint to szone_error as it suggests; gdb
                            does not find the routine.

                            Update 2008/02/21: This bug no longer shows up, but I'm
                            leaving this note in place in case it crops up later.
    */

    int set_breakpoint_here = 42;
}

/******************************************************************************/

void test1()
{
    adobe::any_regular_t empty;
    adobe::any_regular_t boolean(true);
    adobe::any_regular_t number(5);
    adobe::dictionary_t  dictionary;
    adobe::array_t       array;
    adobe::string_t      string("Hello, world!");
    my_custom_type_t     custom_type;
    adobe::any_regular_t custom_regular(custom_type);

    dictionary[adobe::static_name_t("key_0")].assign(custom_type);
    dictionary[adobe::static_name_t("key_1")].assign(42);
    dictionary[adobe::static_name_t("key_2")].assign(string);

    array.push_back(number);
    array.push_back(adobe::any_regular_t(string));

    const adobe::any_regular_t& any_regular_reference(number);
    const adobe::dictionary_t&  dictionary_reference(dictionary);
    const adobe::array_t&       array_reference(array);
    const adobe::string_t&      string_reference(string);
    const adobe::any_regular_t& custom_regular_reference(custom_regular);

    int set_breakpoint_here = 42;
}

/******************************************************************************/

int main(int argc, char *argv[])
try
{
    test0();
    test1();
}
catch (const std::exception& error)
{
    std::cerr << "Top-level exception: " << error.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Top-level exception: unknown" << std::endl;
    return 1;
}

/******************************************************************************/
