/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/**************************************************************************************************/

#include <adobe/config.hpp>

#define ADOBE_DLL_SAFE 0

#include <adobe/future/widgets/headers/virtual_machine_extension.hpp>

#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/future/widgets/headers/widget_tokens.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

#include <boost/gil/image.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

void throw_function_not_defined(adobe::name_t function_name)
{
    throw std::logic_error(
        adobe::make_string("Function \'", function_name.c_str(), "\' not defined."));
}

adobe::any_regular_t contributing_adaptor(const adobe::sheet_t& sheet, const adobe::array_t& array)
{
    if (array.size() != 1) throw std::logic_error("contributing takes a single argument.");
    return adobe::any_regular_t(sheet.contributing_to_cell(array[0].cast<adobe::name_t>()));
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

any_regular_t vm_dictionary_image_proc(const dictionary_t& named_argument_set)
{
    if (named_argument_set.empty())
        return any_regular_t(empty_t());

    std::string               filename;
    boost::gil::rgba8_image_t the_image;

    get_value(named_argument_set, key_name, filename);

    if (!filename.empty())
        image_slurp(boost::filesystem::path(filename), the_image);

    return any_regular_t(the_image);
}

/**************************************************************************************************/

any_regular_t vm_array_image_proc(const array_t& argument_set)
{
    if (argument_set.empty())
        return any_regular_t(empty_t());

    std::string               filename;
    boost::gil::rgba8_image_t the_image;

    argument_set[0].cast(filename);

    if (!filename.empty())
        image_slurp(boost::filesystem::path(filename), the_image);

    return any_regular_t(the_image);
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const boost::gil::rgba8_image_t& /*image*/)
{
    s << "FIXME: " << __FILE__ << __LINE__ << ": "
      << "operator<<(std::ostream& s, const boost::gil::rgba8_image_t& image)"
      << std::endl;

    return s;
}

/**************************************************************************************************/

any_regular_t asl_standard_dictionary_function_lookup(name_t              function_name,
                                                      const dictionary_t& named_argument_set)
{
    if (function_name == "image"_name)
    {
        return implementation::vm_dictionary_image_proc(named_argument_set);
    }
    else
    {
        throw_function_not_defined(function_name);
    }

    return any_regular_t(empty_t());
}

/**************************************************************************************************/

any_regular_t asl_standard_array_function_lookup(name_t         function_name,
                                                 const array_t& argument_set)
{
    if (function_name == "image"_name)
    {
        return implementation::vm_array_image_proc(argument_set);
    }
    else
    {
        throw_function_not_defined(function_name);
    }

    return any_regular_t(empty_t());
}

/**************************************************************************************************/
#if 0
#pragma mark -
#endif
/**************************************************************************************************/

vm_lookup_t::vm_lookup_t()
{
    insert_dictionary_function("image"_name, &implementation::vm_dictionary_image_proc);
    insert_array_function("image"_name, &implementation::vm_array_image_proc);
}

/**************************************************************************************************/

void vm_lookup_t::attach_to(sheet_t& sheet)
{
    insert_array_function("contributing"_name,
        boost::bind(&contributing_adaptor, boost::cref(sheet), _1));
        
    variable_lookup_m = boost::bind(&sheet_t::get, &sheet, _1);
}

/**************************************************************************************************/

void vm_lookup_t::insert_dictionary_function(name_t name, const dictionary_function_t& proc)
{ dmap_m.insert(dictionary_function_map_t::value_type(name, proc)); }

/**************************************************************************************************/

void vm_lookup_t::insert_array_function(name_t name, const array_function_t& proc)
{ amap_m.insert(array_function_map_t::value_type(name, proc)); }

/**************************************************************************************************/

void vm_lookup_t::attach_to(virtual_machine_t& vm)
{
    vm.set_dictionary_function_lookup(boost::bind(&vm_lookup_t::dproc, boost::cref(*this), _1, _2));
    vm.set_array_function_lookup(boost::bind(&vm_lookup_t::aproc, boost::cref(*this), _1, _2));
    vm.set_variable_lookup(boost::ref(variable_lookup_m));
}

/**************************************************************************************************/

any_regular_t vm_lookup_t::dproc(name_t name, const dictionary_t& argument_set) const
{
    dictionary_function_map_t::const_iterator found(dmap_m.find(name));

    if (found != dmap_m.end())
        return found->second(argument_set);

    throw std::runtime_error(adobe::make_string("DFunction ", name.c_str(), " not found."));
}

/**************************************************************************************************/

any_regular_t vm_lookup_t::aproc(name_t name, const array_t& argument_set) const
{
    array_function_map_t::const_iterator found(amap_m.find(name));

    if (found != amap_m.end())
        return found->second(argument_set);

    throw std::runtime_error(adobe::make_string("AFunction ", name.c_str(), " not found."));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
