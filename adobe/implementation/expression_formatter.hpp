/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/******************************************************************************/

#ifndef ADOBE_EXPRESSION_FORMATTER_HPP
#define ADOBE_EXPRESSION_FORMATTER_HPP

/******************************************************************************/

#include <adobe/config.hpp>

#include <boost/function.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/implementation/token.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/static_table.hpp>
#include <adobe/string.hpp>

/******************************************************************************/

namespace adobe {

/******************************************************************************/

class spaces : public basic_omanipulator<std::size_t, char, std::char_traits<char> >
{
    typedef basic_omanipulator<std::size_t, char, std::char_traits<char> > inherited_t;

public:
    typedef inherited_t::stream_type    stream_type;
    typedef inherited_t::argument_type  argument_type;

    spaces(argument_type num) :
        inherited_t(spaces::fct, num)
        { }

    inherited_t& operator() (argument_type i)
        { arg_m = i; return *this; }

private:
    static stream_type& fct(stream_type& os, const argument_type& i)
    {
        for (argument_type count(0); count < i; ++count)
            os.put(' ');

        return os;
    }
};

/******************************************************************************/

namespace implementation {

/******************************************************************************/

struct expression_formatter_t
{
    expression_formatter_t();

    std::string format(const array_t& expression,
                       std::size_t    indent,
                       bool           tight);

private:
    typedef boost::function<void ()>             operator_t;
    typedef static_table<name_t, operator_t, 21> operator_table_t;

    void unary_operation(const char* operation);
    void binary_operation(const char* operation);

    void op_ifelse();
    void op_index();
    void op_function();
    void op_array();
    void op_dictionary();
    void op_variable();

    std::string strip_expression(const std::string& expr_str);
    std::string add_indentation(const std::string& expr_str, std::size_t indent = 4);

    void assert_stack_ok() const;

    std::vector<std::string> stack_m;
    operator_table_t         operator_table_m;
    std::size_t              indent_m;
    bool                     tight_m;
};

/******************************************************************************/

} // namespace implementation

/******************************************************************************/
/*!
    @brief "Unparses" a parsed token stream to a CEL-syntax expression

    @param expression The parsed expression as a token vector
    @param indent The number of spaces to indent the output by
    @param tight When true does not add a newline after opening or before
           closing an array or dictionary expression

    @return A CEL-syntax formatted expression representing the original parsed
            string
*/
std::string format_expression(const array_t& expression,
                              std::size_t    indent = 0,
                              bool           tight = false);

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
// ADOBE_EXPRESSION_FORMATTER_HPP
#endif

/******************************************************************************/
