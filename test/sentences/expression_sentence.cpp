/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include "expression_sentence.hpp"

#include <adobe/implementation/expression_parser.hpp>
#include <adobe/virtual_machine.hpp>

#include <typeinfo>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

adobe::static_name_t receives_k("receives");
adobe::static_name_t emits_k("emits");

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

inline void expression_string_parse(const std::string& expression, adobe::array_t& result)
{
    if (expression.empty())
        return;

    std::istringstream stream(expression);

    adobe::expression_parser(stream, adobe::line_position_t("expression_string_parse")).require_expression(result);
}

/**************************************************************************************************/

adobe::any_regular_t simple_variable_lookup(adobe::name_t              variable_name,
                                            const adobe::dictionary_t& attributes)
{
    return get_value(attributes, variable_name);
}

/**************************************************************************************************/

bool value_is_name(const adobe::any_regular_t& value, adobe::name_t name)
{
    return value.type_info() == adobe::type_info<adobe::name_t>() && value.cast<adobe::name_t>() == name;
}

/**************************************************************************************************/

adobe::any_regular_t array_function(adobe::name_t              function_name,
                                    const adobe::array_t&      parameters,
                                    const adobe::dictionary_t& attributes)
{
    if (parameters.empty())
        throw std::runtime_error(adobe::make_string("function ",
                                                    function_name.c_str(),
                                                    ": empty parameter list."));

    if (function_name == adobe::receives_k)
    {
        adobe::any_regular_t value(get_value(attributes, adobe::receives_k));
        bool                 success(false);
        adobe::name_t        thename(parameters.front().cast<adobe::name_t>());

        if (value.type_info() == adobe::type_info<adobe::array_t>())
        {
            // search for concept within the array
        }
        else if (value.type_info() == adobe::type_info<adobe::name_t>())
        {
            success = value_is_name(value, thename);
        }

        return adobe::any_regular_t(success);
    }
    else if (function_name == adobe::emits_k)
    {
        adobe::any_regular_t value(get_value(attributes, adobe::emits_k));
        bool                 success(false);
        adobe::name_t        thename(parameters.front().cast<adobe::name_t>());

        if (value.type_info() == adobe::type_info<adobe::array_t>())
        {
            // search for concept within the array
        }
        else if (value.type_info() == adobe::type_info<adobe::name_t>())
        {
            success = value_is_name(value, thename);
        }

        return adobe::any_regular_t(success);
    }
    else if (function_name == adobe::static_name_t("require"))
    {
        // grab the first parameter, make sure it's a name, then check to make sure it
        // exists in the attribute dictionary. Then take the second parameter and compare
        // it to the value found at attributes[first_parameter], and make sure
        // they're the same. (If the second parameter doesn't exist, presume it's 'true').
        //
        // We could allow for the second parameter to be an expression in an of
        // itself, but that would require access to the VM at this point, which may not be what we
        // want. Something to consider for later on.

        adobe::name_t thename(parameters.front().cast<adobe::name_t>());
        bool          success(attributes.count(thename) != 0);

        if (success)
        {
            adobe::any_regular_t equal_to(true);

            if (parameters.size() > 1)
                equal_to = parameters[1];

            success = get_value(attributes, thename) == equal_to;
        }

        return adobe::any_regular_t(success);
    }

    throw std::runtime_error(adobe::make_string("function ", function_name.c_str(), ": not found."));
}

/**************************************************************************************************/

bool evaluate_prerequisite_expression(const adobe::array_t& expression, const adobe::dictionary_t& attributes)
{
    static adobe::virtual_machine_t vm_s;

    vm_s.set_array_function_lookup(boost::bind(&array_function, _1, _2, boost::ref(attributes)));
    vm_s.set_simple_variable_lookup(boost::bind(&simple_variable_lookup, _1, boost::ref(attributes)));
    vm_s.evaluate(expression);

    bool success(false);

    vm_s.back().value_m.cast(success);
    vm_s.pop_back();

    return success;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

unary_expression_sentence_t::connection_t unary_expression_sentence_t::monitor(const monitor_proc_t& f)
{
    notifier_set_m.push_front(f);

    return notifier_set_m.begin();
}

/**************************************************************************************************/

bool unary_expression_sentence_t::test_subject(const subject_type& subject)
{
    return evaluate_prerequisite_expression(subject_expression_m, subject);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

binary_expression_sentence_t::connection_t binary_expression_sentence_t::monitor(const monitor_proc_t& f)
{
    notifier_set_m.push_front(f);

    return notifier_set_m.begin();
}

/**************************************************************************************************/

bool binary_expression_sentence_t::test_subject(const subject_type& subject)
{
    return evaluate_prerequisite_expression(subject_expression_m, subject);
}

/**************************************************************************************************/

bool binary_expression_sentence_t::test_direct_object(const subject_type& /*subject*/, const direct_object_type& direct_object)
{
    return evaluate_prerequisite_expression(dirobj_expression_m, direct_object);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

poly_sentence_t make_expression_sentence(name_t             verb,
                                         const std::string& subject_requirement,
                                         const std::string& direct_object_requirement)
{
    array_t subject_expr;

    expression_string_parse(subject_requirement, subject_expr);

    if (direct_object_requirement != std::string())
    {
        array_t dirobj_expr;

        expression_string_parse(direct_object_requirement, dirobj_expr);

        poly_binary_sentence_t binary_sentence(binary_expression_sentence_t(verb, subject_expr, dirobj_expr));

        return poly_cast<poly_sentence_t&>(binary_sentence);
    }

    return poly_sentence_t(unary_expression_sentence_t(verb, subject_expr));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
