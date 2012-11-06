/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <adobe/config.hpp>

#include <sstream>

#include <boost/bind.hpp>
#include <boost/next_prior.hpp>

#include <adobe/algorithm.hpp>
#include <adobe/implementation/expression_formatter.hpp>

#define ADOBE_EXPRESSION_FILTER_DEBUG !defined(NDEBUG) && 0

#if ADOBE_EXPRESSION_FILTER_DEBUG
    #include <adobe/iomanip_asl_cel.hpp>
#endif

#if defined(BOOST_WINDOWS)
namespace std {
    using ::isspace;
} // namespace std
#endif

/******************************************************************************/

namespace adobe {

/******************************************************************************/

namespace implementation {

/******************************************************************************/

expression_formatter_t::expression_formatter_t() :
    indent_m(0)
{
    typedef operator_table_t::entry_type op_entry_type;

    operator_table_t table =
    {{
        op_entry_type(not_k,           boost::bind(&expression_formatter_t::unary_operation, boost::ref(*this), "!")),
        op_entry_type(unary_negate_k,  boost::bind(&expression_formatter_t::unary_operation, boost::ref(*this), "-")),
        op_entry_type(add_k,           boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "+")),
        op_entry_type(subtract_k,      boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "-")),
        op_entry_type(multiply_k,      boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "*")),
        op_entry_type(modulus_k,       boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "%")),
        op_entry_type(divide_k,        boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "/")),
        op_entry_type(less_k,          boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "<")),
        op_entry_type(greater_k,       boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), ">")),
        op_entry_type(less_equal_k,    boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "<=")),
        op_entry_type(greater_equal_k, boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), ">=")),
        op_entry_type(equal_k,         boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "==")),
        op_entry_type(not_equal_k,     boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "!=")),
        op_entry_type(ifelse_k,        boost::bind(&expression_formatter_t::op_ifelse, boost::ref(*this))),
        op_entry_type(index_k,         boost::bind(&expression_formatter_t::op_index, boost::ref(*this))),
        op_entry_type(function_k,      boost::bind(&expression_formatter_t::op_function, boost::ref(*this))),
        op_entry_type(array_k,         boost::bind(&expression_formatter_t::op_array, boost::ref(*this))),
        op_entry_type(dictionary_k,    boost::bind(&expression_formatter_t::op_dictionary, boost::ref(*this))),
        op_entry_type(variable_k,      boost::bind(&expression_formatter_t::op_variable, boost::ref(*this))),
        op_entry_type(and_k,           boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "&&")),
        op_entry_type(or_k,            boost::bind(&expression_formatter_t::binary_operation, boost::ref(*this), "||"))
    }};

    table.sort();

    operator_table_m = table;
}

/******************************************************************************/

void expression_formatter_t::assert_stack_ok() const
{
    if (stack_m.empty())
        throw std::runtime_error("Expression invalid.");
}

/******************************************************************************/

string_t expression_formatter_t::format(const array_t& expression,
                                        std::size_t    indent,
                                        bool           tight)
{
    indent_m = indent;
    tight_m = tight;

#if ADOBE_EXPRESSION_FILTER_DEBUG
    if (stack_m.size() != 1)
    {
        std::cerr << "Token stream: ";
        std::cerr << begin_asl_cel_unsafe << expression << end_asl_cel_unsafe << std::endl;
    }
#endif

    for(array_t::const_iterator iter(expression.begin()); iter != expression.end(); ++iter)
    {
        if (iter->type_info() == type_info<name_t>() && iter->cast<name_t>().c_str()[0] == '.')
            operator_table_m(iter->cast<name_t>())();
        else
        {
            std::stringstream stream;

            if (iter->type_info() == type_info<name_t>())
                stream << "@" << *iter;
            else if (iter->type_info() == type_info<string_t>())
                stream << "\"" << *iter << "\"";
            else if (iter->type_info() == type_info<array_t>())
            {
                const array_t& array(iter->cast<array_t>());

                if (array.empty())
                    stream << "[ ]";
                else
                    stream << format_expression(array, indent_m + 4);
            }
            else if (iter->type_info() == type_info<dictionary_t>())
                stream << "{ }";
            else if (iter->type_info() == type_info<bool>())
                stream << (iter->cast<bool>() ? "true" : "false");
            else
                stream << *iter;

#if ADOBE_EXPRESSION_FILTER_DEBUG
            const std::string& to_push(stream.str());

            stack_m.push_back(to_push);
#else
            stack_m.push_back(stream.str());
#endif
        }
    }

#if ADOBE_EXPRESSION_FILTER_DEBUG
    if (stack_m.size() != 1)
    {
        std::cerr << "Remainder of token stream: ";
        copy(stack_m, std::ostream_iterator<string_t>(std::cerr, " << "));
    }
#endif

    if (stack_m.size() != 1)
        throw std::runtime_error("Invalid expression token stream");

    string_t result(strip_expression(stack_m.back()));

    stack_m.pop_back();

    return result;
}

/******************************************************************************/

void expression_formatter_t::unary_operation(const char* operation)
{
    assert_stack_ok();
    string_t operand(stack_m.back());
    stack_m.pop_back();

    std::stringstream stream;

    stream << operation << operand;

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::binary_operation(const char* operation)
{
    assert_stack_ok();
    string_t operand2(stack_m.back());
    stack_m.pop_back();

    assert_stack_ok();
    string_t operand1(stack_m.back());
    stack_m.pop_back();

    std::stringstream stream;

    stream << "(" << operand1 << " " << operation << " " << operand2 << ")";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_ifelse()
{
    assert_stack_ok();
    string_t expr2(stack_m.back());
    stack_m.pop_back();

    assert_stack_ok();
    string_t expr1(stack_m.back());
    stack_m.pop_back();

    assert_stack_ok();
    string_t test(stack_m.back());
    stack_m.pop_back();

    std::stringstream stream;

    stream << "("
           << strip_expression(test) << " ? "
           << strip_expression(expr1) << " : "
           << strip_expression(expr2)
           << ")";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_index()
{
    assert_stack_ok();
    string_t index(stack_m.back());
    stack_m.pop_back();

    assert_stack_ok();
    string_t variable(stack_m.back());
    stack_m.pop_back();

    std::stringstream stream;

    if (index.c_str()[0] == '@')
    {
        index = index.c_str() + 1;
        stream << variable << "." << index;
    }
    else
        stream << variable << "[" << strip_expression(index) << "]";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_function()
{
    assert_stack_ok();
    string_t function_name(stack_m.back());
    stack_m.pop_back();

    assert_stack_ok();
    std::string parameter_set(stack_m.back());
    stack_m.pop_back();

    function_name = function_name.c_str() + 1;

    std::string::iterator first(parameter_set.begin());
    std::string::iterator last(parameter_set.end());

    while (std::isspace(*++first) && first != last);
    while (std::isspace(*--last) && first != last);

    parameter_set = std::string(first, last);

    std::stringstream stream;

    stream << function_name << "(";

    std::size_t function_indent(stream.str().size());

    stream << add_indentation(parameter_set, function_indent - 2) << ")";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_array()
{
    assert_stack_ok();
    long count(std::atoi(stack_m.back().c_str()));
    stack_m.pop_back();

    std::reverse(boost::prior(stack_m.end(), count), stack_m.end());

    std::stringstream stream;

    indent_m += tight_m ? 2 : 4;

    stream << "[";
    
    if (!tight_m)
        stream << std::endl;
    else
        stream << ' ';

    for (long i(0); i < count; ++i)
    {
        if (i)
            stream << "," << std::endl;

        if (!tight_m || i)
            stream << adobe::spaces(indent_m);

        assert_stack_ok();
        stream << add_indentation(strip_expression(stack_m.back()), 2);
        stack_m.pop_back();
    }

    indent_m -= tight_m ? 2 : 4;

    if (!tight_m)
        stream << std::endl << adobe::spaces(indent_m);
    else
        stream << ' ';

    stream << "]";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_dictionary()
{
    assert_stack_ok();
    long count(std::atoi(stack_m.back().c_str()));
    stack_m.pop_back();

    // collect the kv pairs for later reversal and concatenation
    vector<string_t> kv_pair_set;

    for (long i(0); i < count; ++i)
    {
        std::stringstream stream;

        assert_stack_ok();
        std::string value(stack_m.back());
        stack_m.pop_back();

        assert_stack_ok();
        string_t key(stack_m.back());
        stack_m.pop_back();

        key = key.c_str() + 1;

        stream << key << ":";

        if (tight_m)
            stream << " ";
        else if ((value[0] == '[' && value != std::string("[ ]")) ||
                 (value[0] == '{' && value != std::string("{ }")))
            stream << std::endl << adobe::spaces(indent_m);
        else
            stream << " ";

        // key + strlen(": { ")
        std::size_t value_indent(std::strlen(key.c_str()) + 4);

        stream << add_indentation(strip_expression(value), value_indent);

        kv_pair_set.push_back(stream.str());
    }

    reverse(kv_pair_set);

    std::stringstream stream;

    stream << "{";

    if (!tight_m)
        stream << std::endl;
    else
        stream << ' ';

    indent_m += tight_m ? 2 : 4;

    for (long i(0); i < count; ++i)
    {
        if (i)
            stream << "," << std::endl;

        if (!tight_m || i)
            stream << adobe::spaces(indent_m);

        stream << strip_expression(kv_pair_set[i]);
    }

    indent_m -= tight_m ? 2 : 4;

    if (!tight_m)
        stream << std::endl << adobe::spaces(indent_m);
    else
        stream << ' ';

    stream << "}";

    stack_m.push_back(stream.str());
}

/******************************************************************************/

void expression_formatter_t::op_variable()
{
    assert_stack_ok();
    string_t var(stack_m.back());
    stack_m.pop_back();

    var = var.c_str() + 1;

    stack_m.push_back(var);
}

/******************************************************************************/

string_t expression_formatter_t::strip_expression(const string_t& expr_str)
{
    if (expr_str.size() < 2)
        return expr_str;

    const char* front(expr_str.c_str());
    const char* back(boost::next(front, expr_str.size() - 1));

    return *front == '(' && *back == ')' ?
               string_t(boost::next(front), back) :
               expr_str;
}

/******************************************************************************/

string_t expression_formatter_t::add_indentation(const string_t& expr_str,
                                                 std::size_t     indent)
{
    // goes looking for the string "\n    " and replaces it with "\n        "

    static const std::string needle("\n    ");
    static const std::size_t needle_size(needle.size());

    std::stringstream new_needle;

    new_needle << '\n' << adobe::spaces(4 + indent);

    std::string            result(expr_str);
    std::string::size_type n(result.find(needle));

    while (n != std::string::npos)
    {
        result.replace(n, needle_size, new_needle.str());

        n = result.find(needle, n + 1);
    }

    return result;
}

/******************************************************************************/

} // namespace implementation

/******************************************************************************/

string_t format_expression(const array_t& expression,
                           std::size_t    indent,
                           bool           tight)
{
    return expression.empty() ?
           string_t() :
           implementation::expression_formatter_t().format(expression,
                                                           indent,
                                                           tight);
}

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
