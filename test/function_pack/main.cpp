/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <iostream>
#include <sstream>

#include <adobe/function_pack.hpp>

/**************************************************************************************************/

#define TEST_ARITY_0_SUITE 1
#define TEST_ARITY_1_SUITE 1
#define TEST_ARITY_2_SUITE 1
#define TEST_ARITY_3_SUITE 1
#define TEST_ARITY_4_SUITE 1
#define TEST_ARITY_5_SUITE 1
#define TEST_ARITY_6_SUITE 1
#define TEST_ARITY_7_SUITE 1

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <typename T>
inline void test_pack(adobe::function_pack_t& pack, adobe::name_t function, const T& argument_set)
{
    static adobe::virtual_machine_t vm;
    std::stringstream               result;

    try
    {
        attach(vm, pack);

        adobe::array_t expression;

        expression.push_back(adobe::any_regular_t(argument_set));
        expression.push_back(adobe::any_regular_t(function));
        expression.push_back(adobe::any_regular_t(adobe::static_name_t(".function")));

        vm.evaluate(expression);

        result << vm.back().value_m;

        vm.pop_back();
    }
    catch (const std::exception& error)
    {
        result << "\"Exception: " << error.what() << '"';
    }
    catch (...)
    {
        result << "\"Exception: unknown\"";
    }

    std::cout << "    call: " << function.c_str()
              << ", arg_set_type: '" << adobe::type_info<T>().name()
              << "', arg_set_empty: " << std::boolalpha << argument_set.empty()
              << ", result: " << result.str() << std::endl;
}

/**************************************************************************************************/

template <typename T>
inline void insert_argument(adobe::array_t& array, T value)
{ array.push_back(adobe::any_regular_t(value)); }

template <typename T>
inline void insert_argument(adobe::dictionary_t& dictionary, adobe::name_t name, T value)
{ dictionary.insert(adobe::dictionary_t::value_type(name, adobe::any_regular_t(value))); }

/**************************************************************************************************/

// function names
const adobe::name_t name_fn_ptr(adobe::static_name_t("fn_ptr"));
const adobe::name_t name_fn_obj(adobe::static_name_t("fn_obj"));
const adobe::name_t name_mem_fn(adobe::static_name_t("mem_fn"));
const adobe::name_t name_cmem_fn(adobe::static_name_t("cmem_fn"));

// named parameters
const adobe::name_t name_this(adobe::static_name_t("this"));
const adobe::name_t name_integer(adobe::static_name_t("int"));
const adobe::name_t name_string(adobe::static_name_t("str"));
const adobe::name_t name_char_ptr(adobe::static_name_t("char_ptr"));
const adobe::name_t name_integer_ptr(adobe::static_name_t("int_ptr"));
const adobe::name_t name_name(adobe::static_name_t("name"));
const adobe::name_t name_double(adobe::static_name_t("double"));
const adobe::name_t name_double_ptr(adobe::static_name_t("double_ptr"));
const adobe::name_t name_name_ptr(adobe::static_name_t("name_ptr"));

// sample data
const std::string    sample_string("Hello");
const char*          sample_char_ptr(", World!");
const long           sample_integer(42);
const long*          sample_integer_ptr(&sample_integer);
const adobe::name_t  sample_name("sample_name");
const double         sample_double(3.1415);
const double*        sample_double_ptr(&sample_double);
const adobe::name_t* sample_name_ptr(&sample_name);

/**************************************************************************************************/

struct struct_t : boost::equality_comparable<struct_t>
{
    typedef const std::string&   arg1_type;
    typedef const long*          arg2_type;
    typedef adobe::name_t        arg3_type;
    typedef double               arg4_type;
    typedef const double*        arg5_type;
    typedef const adobe::name_t* arg6_type;

    explicit struct_t(std::size_t a1 = sample_integer) :
        a1_m(a1)
    { }

    // member function, arity 1
    double mem_fn_0()
    { return a1_m * 2; }

    // const member function, arity 1
    double cmem_fn_0() const
    { return a1_m * 2; }

    // member function, arity 2
    double mem_fn_1(arg1_type arg1)
    { return mem_fn_0() + arg1.size(); }

    // const member function, arity 2
    double cmem_fn_1(arg1_type arg1) const
    { return cmem_fn_0() + arg1.size(); }

    // member function, arity 3
    double mem_fn_2(arg1_type arg1, arg2_type arg2)
    { assert(arg2); return mem_fn_1(arg1) + *arg2; }

    // const member function, arity 3
    double cmem_fn_2(arg1_type arg1, arg2_type arg2) const
    { assert(arg2); return cmem_fn_1(arg1) + *arg2; }

    // member function, arity 4
    double mem_fn_3(arg1_type arg1, arg2_type arg2, arg3_type arg3)
    { return mem_fn_2(arg1, arg2) + std::strlen(arg3.c_str()); }

    // const member function, arity 4
    double cmem_fn_3(arg1_type arg1, arg2_type arg2, arg3_type arg3) const
    { return cmem_fn_2(arg1, arg2) + std::strlen(arg3.c_str()); }

    // member function, arity 5
    double mem_fn_4(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4)
    { return mem_fn_3(arg1, arg2, arg3) + arg4; }

    // const member function, arity 5
    double cmem_fn_4(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4) const
    { return cmem_fn_3(arg1, arg2, arg3) + arg4; }

    // member function, arity 6
    double mem_fn_5(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5)
    { assert(arg5); return mem_fn_4(arg1, arg2, arg3, arg4) + *arg5; }

    // const member function, arity 6
    double cmem_fn_5(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4,
                     arg5_type arg5) const
    { assert(arg5); return cmem_fn_4(arg1, arg2, arg3, arg4) + *arg5; }

    // member function, arity 7
    double mem_fn_6(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5,
                    arg6_type arg6)
    { assert(arg6); return mem_fn_5(arg1, arg2, arg3, arg4, arg5) + std::strlen(arg6->c_str()); }

    // const member function, arity 7
    double cmem_fn_6(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4,
                     arg5_type arg5, arg6_type arg6) const
    { assert(arg6); return cmem_fn_5(arg1, arg2, arg3, arg4, arg5) + std::strlen(arg6->c_str()); }

    std::size_t a1_m;
};

bool operator==(const struct_t&, const struct_t&)
{ return true; }

/**************************************************************************************************/

void test_pack_all(adobe::function_pack_t& pack)
{
    static struct_t                  my_struct;
    static adobe::array_t            unnamed_argument_set;
    static adobe::dictionary_t       named_argument_set;
    static adobe::array_t            member_unnamed_argument_set;
    static adobe::dictionary_t       member_named_argument_set;
    static bool                      inited(false);
    static const adobe::array_t      empty_unnamed_argument_set;
    static const adobe::dictionary_t empty_named_argument_set;

    if (!inited)
    {
        inited = true;

        insert_argument(unnamed_argument_set, sample_integer);
        insert_argument(unnamed_argument_set, sample_string);
        insert_argument(unnamed_argument_set, sample_integer_ptr);
        insert_argument(unnamed_argument_set, sample_name);
        insert_argument(unnamed_argument_set, sample_double);
        insert_argument(unnamed_argument_set, sample_double_ptr);
        insert_argument(unnamed_argument_set, sample_name_ptr);

        insert_argument(named_argument_set, name_this, &my_struct);
        insert_argument(named_argument_set, name_integer, sample_integer);
        insert_argument(named_argument_set, name_string, sample_string);
        insert_argument(named_argument_set, name_integer_ptr, sample_integer_ptr);
        insert_argument(named_argument_set, name_name, sample_name);
        insert_argument(named_argument_set, name_double, sample_double);
        insert_argument(named_argument_set, name_double_ptr, sample_double_ptr);
        insert_argument(named_argument_set, name_name_ptr, sample_name_ptr);

        insert_argument(member_unnamed_argument_set, &my_struct);
        insert_argument(member_unnamed_argument_set, sample_string);
        insert_argument(member_unnamed_argument_set, sample_integer_ptr);
        insert_argument(member_unnamed_argument_set, sample_name);
        insert_argument(member_unnamed_argument_set, sample_double);
        insert_argument(member_unnamed_argument_set, sample_double_ptr);
        insert_argument(member_unnamed_argument_set, sample_name_ptr);
    }

    std::cout << "  Should pass:" << std::endl;

    test_pack(pack, name_fn_ptr, unnamed_argument_set);
    test_pack(pack, name_fn_obj, unnamed_argument_set);
    test_pack(pack, name_mem_fn, member_unnamed_argument_set);
    test_pack(pack, name_cmem_fn, member_unnamed_argument_set);

    test_pack(pack, name_fn_ptr, named_argument_set);
    test_pack(pack, name_fn_obj, named_argument_set);
    test_pack(pack, name_mem_fn, named_argument_set);
    test_pack(pack, name_cmem_fn, named_argument_set);

    std::cout << "  Should fail:" << std::endl;

    test_pack(pack, name_fn_ptr, empty_unnamed_argument_set);
    test_pack(pack, name_fn_obj, empty_unnamed_argument_set);
    test_pack(pack, name_mem_fn, empty_unnamed_argument_set);
    test_pack(pack, name_cmem_fn, empty_unnamed_argument_set);

    test_pack(pack, name_fn_ptr, empty_named_argument_set);
    test_pack(pack, name_fn_obj, empty_named_argument_set);
    test_pack(pack, name_mem_fn, empty_named_argument_set);
    test_pack(pack, name_cmem_fn, empty_named_argument_set);
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

ADOBE_NAME_TYPE_0("struct_t", struct_t);

/**************************************************************************************************/

namespace {

/**************************************************************************************************/
#if TEST_ARITY_0_SUITE
namespace arity_0 {

/**************************************************************************************************/

double free() // free function
{
    struct_t my_struct;

    return my_struct.mem_fn_0();
}

/**************************************************************************************************/

void test()
{
    boost::function<double ()> fn_obj(&free);
    adobe::function_pack_t  pack;
    adobe::array_t          argument_set;

    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);

    std::cout << "  Should pass:" << std::endl;

    test_pack(pack, name_fn_ptr, argument_set);
    test_pack(pack, name_fn_obj, argument_set);

    std::cout << "  Should fail:" << std::endl;

    test_pack(pack, name_mem_fn, argument_set);
    test_pack(pack, name_cmem_fn, adobe::dictionary_t());
}

/**************************************************************************************************/

} // namespace arity_0
#endif
/**************************************************************************************************/
#if TEST_ARITY_1_SUITE
namespace arity_1 {

/**************************************************************************************************/

double free(int x) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_0();
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_0);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_0);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer);
    pack.register_function(name_fn_obj, fn_obj, name_integer);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_0, name_this);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_0, name_this);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_1
#endif
/**************************************************************************************************/
#if TEST_ARITY_2_SUITE
namespace arity_2 {

/**************************************************************************************************/

double free(int x, struct_t::arg1_type arg1) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_1(arg1);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int, struct_t::arg1_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_1);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_1);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_1, name_this, name_string);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_1, name_this, name_string);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_2
#endif
/**************************************************************************************************/
#if TEST_ARITY_3_SUITE
namespace arity_3 {

/**************************************************************************************************/

double free(int x, struct_t::arg1_type arg1, struct_t::arg2_type arg2) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_2(arg1, arg2);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int, struct_t::arg1_type, struct_t::arg2_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_2);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_2);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string, name_integer_ptr);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string, name_integer_ptr);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_2, name_this, name_string,
                           name_integer_ptr);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_2, name_this, name_string,
                           name_integer_ptr);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_3
#endif
/**************************************************************************************************/
#if TEST_ARITY_4_SUITE
namespace arity_4 {

/**************************************************************************************************/

double free(int x,
            struct_t::arg1_type arg1,
            struct_t::arg2_type arg2,
            struct_t::arg3_type arg3) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_3(arg1, arg2, arg3);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int,
                            struct_t::arg1_type,
                            struct_t::arg2_type,
                            struct_t::arg3_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_3);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_3);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string, name_integer_ptr,
                           name_name);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string, name_integer_ptr,
                           name_name);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_3, name_this, name_string,
                           name_integer_ptr, name_name);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_3, name_this, name_string,
                           name_integer_ptr, name_name);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_4
#endif
/**************************************************************************************************/
#if TEST_ARITY_5_SUITE
namespace arity_5 {

/**************************************************************************************************/

double free(int x,
            struct_t::arg1_type arg1,
            struct_t::arg2_type arg2,
            struct_t::arg3_type arg3,
            struct_t::arg4_type arg4) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_4(arg1, arg2, arg3, arg4);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int,
                            struct_t::arg1_type,
                            struct_t::arg2_type,
                            struct_t::arg3_type,
                            struct_t::arg4_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_4);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_4);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string, name_integer_ptr,
                           name_name, name_double);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string, name_integer_ptr,
                           name_name, name_double);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_4, name_this, name_string,
                           name_integer_ptr, name_name, name_double);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_4, name_this, name_string,
                           name_integer_ptr, name_name, name_double);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_5
#endif
/**************************************************************************************************/
#if TEST_ARITY_6_SUITE
namespace arity_6 {

/**************************************************************************************************/

double free(int x,
            struct_t::arg1_type arg1,
            struct_t::arg2_type arg2,
            struct_t::arg3_type arg3,
            struct_t::arg4_type arg4,
            struct_t::arg5_type arg5) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_5(arg1, arg2, arg3, arg4, arg5);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int,
                            struct_t::arg1_type,
                            struct_t::arg2_type,
                            struct_t::arg3_type,
                            struct_t::arg4_type,
                            struct_t::arg5_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_5);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_5);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string, name_integer_ptr,
                           name_name, name_double, name_double_ptr);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string, name_integer_ptr,
                           name_name, name_double, name_double_ptr);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_5, name_this, name_string,
                           name_integer_ptr, name_name, name_double, name_double_ptr);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_5, name_this, name_string,
                           name_integer_ptr, name_name, name_double, name_double_ptr);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_6
#endif
/**************************************************************************************************/
#if TEST_ARITY_7_SUITE
namespace arity_7 {

/**************************************************************************************************/

double free(int x,
            struct_t::arg1_type arg1,
            struct_t::arg2_type arg2,
            struct_t::arg3_type arg3,
            struct_t::arg4_type arg4,
            struct_t::arg5_type arg5,
            struct_t::arg6_type arg6) // free function
{
    struct_t my_struct(x);

    return my_struct.mem_fn_6(arg1, arg2, arg3, arg4, arg5, arg6);
}

/**************************************************************************************************/

void test()
{
    boost::function<double (int,
                            struct_t::arg1_type,
                            struct_t::arg2_type,
                            struct_t::arg3_type,
                            struct_t::arg4_type,
                            struct_t::arg5_type,
                            struct_t::arg6_type)> fn_obj(&free);
    adobe::function_pack_t pack;

    // unnamed function registration
    pack.register_function(name_fn_ptr, &free);
    pack.register_function(name_fn_obj, fn_obj);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_6);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_6);

    // named function registration
    pack.register_function(name_fn_ptr, &free, name_integer, name_string, name_integer_ptr,
                           name_name, name_double, name_double_ptr, name_name_ptr);
    pack.register_function(name_fn_obj, fn_obj, name_integer, name_string, name_integer_ptr,
                           name_name, name_double, name_double_ptr, name_name_ptr);
    pack.register_function(name_mem_fn, &struct_t::mem_fn_6, name_this, name_string,
                           name_integer_ptr, name_name, name_double, name_double_ptr, name_name_ptr);
    pack.register_function(name_cmem_fn, &struct_t::cmem_fn_6, name_this, name_string,
                           name_integer_ptr, name_name, name_double, name_double_ptr, name_name_ptr);

    test_pack_all(pack);
}

/**************************************************************************************************/

} // namespace arity_7
#endif
/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main()
try
{
#if TEST_ARITY_0_SUITE
    std::cout << "Arity 0 Test Suite:" << std::endl;
    arity_0::test();
#endif

#if TEST_ARITY_1_SUITE
    std::cout << "Arity 1 Test Suite:" << std::endl;
    arity_1::test();
#endif

#if TEST_ARITY_2_SUITE
    std::cout << "Arity 2 Test Suite:" << std::endl;
    arity_2::test();
#endif

#if TEST_ARITY_3_SUITE
    std::cout << "Arity 3 Test Suite:" << std::endl;
    arity_3::test();
#endif

#if TEST_ARITY_4_SUITE
    std::cout << "Arity 4 Test Suite:" << std::endl;
    arity_4::test();
#endif

#if TEST_ARITY_5_SUITE
    std::cout << "Arity 5 Test Suite:" << std::endl;
    arity_5::test();
#endif

#if TEST_ARITY_6_SUITE
    std::cout << "Arity 6 Test Suite:" << std::endl;
    arity_6::test();
#endif

#if TEST_ARITY_7_SUITE
    std::cout << "Arity 7 Test Suite:" << std::endl;
    arity_7::test();
#endif

    return 0;
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return 0;
}
catch (...)
{
    std::cerr << "Exception: unknown" << std::endl;

    return 0;
}

/**************************************************************************************************/
