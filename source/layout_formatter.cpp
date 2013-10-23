/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <adobe/config.hpp>

#include <cctype>
#include <sstream>

#include <boost/bind.hpp>

#include <adobe/eve_parser.hpp>
#include <adobe/formatter_tokens.hpp>
#include <adobe/implementation/expression_formatter.hpp>
#include <adobe/layout_formatter.hpp>

/******************************************************************************/

namespace {

/******************************************************************************/

using namespace adobe;

/******************************************************************************/

void callback_unhandled(const std::string& callback)
{
    std::cerr << "TODO: callback_unhandled: " << callback << std::endl;
}

/******************************************************************************/

struct eve_node_forest_t
{
public:
    typedef eve_callback_suite_t::position_t  position_t;
    typedef eve_callback_suite_t::cell_type_t cell_type_t;

    eve_node_forest_t(std::istream& eve_stream, const line_position_t& line_position)
    {
        suite_m.add_view_proc_m = boost::bind(&eve_node_forest_t::add_view,
                                              boost::ref(*this), _1, _2, _3, _4, _5, _6);
        suite_m.add_cell_proc_m = boost::bind(&eve_node_forest_t::add_cell,
                                              boost::ref(*this), _1, _2, _3, _4, _5, _6);

		// TODO : Need to handle interface cell, logic cells, and relate clauses
        suite_m.finalize_sheet_proc_m = boost::bind(&callback_unhandled, "finalize_sheet_proc_m");
        suite_m.add_relation_proc_m = boost::bind(&callback_unhandled, "add_relation_proc_m");
        suite_m.add_interface_proc_m = boost::bind(&callback_unhandled, "add_interface_proc_m");

        position_t root(node_forest_m.begin());

        parse(eve_stream, line_position, root, suite_m);
    }

    forest<dictionary_t> node_forest_m;
    vector<dictionary_t> cell_set_m;

private:
    position_t add_view(const position_t&      parent,
                        const line_position_t& parse_location,
                        name_t                 name,
                        const array_t&         parameters,
                        const std::string&     brief,
                        const std::string&     detailed);

    void add_cell(cell_type_t            type,
                  name_t                 name,
                  const line_position_t& position,
                  const array_t&         initializer,
                  const std::string&     brief,
                  const std::string&     detailed);

    typedef forest<dictionary_t> forest_t;
    typedef forest_t::iterator   iterator;

    eve_callback_suite_t suite_m;
};

/******************************************************************************/

eve_node_forest_t::position_t eve_node_forest_t::add_view(const position_t&      parent,
                                                          const line_position_t& /*parse_location*/,
                                                          name_t                 name,
                                                          const array_t&         parameters,
                                                          const std::string&     brief,
                                                          const std::string&     detailed)
{
    iterator parent_node_iterator(boost::any_cast<iterator>(parent));

    dictionary_t node;

    node[key_name] = any_regular_t(name);
    node[key_parameters] = any_regular_t(parameters);
    node[key_comment_brief] = any_regular_t(brief);
    node[key_comment_detailed] = any_regular_t(detailed);

    iterator node_iterator(node_forest_m.insert(trailing_of(parent_node_iterator), node));

    return position_t(node_iterator);
}

/******************************************************************************/

void eve_node_forest_t::add_cell(cell_type_t            type,
                                 name_t                 name,
                                 const line_position_t& /*position*/,
                                 const array_t&         initializer,
                                 const std::string&     brief,
                                 const std::string&     detailed)
{
    dictionary_t cell;

    static const name_t name_interface(static_name_t("interface"));
    static const name_t name_constant(static_name_t("constant"));

    if (type == eve_callback_suite_t::interface_k)
        cell[key_cell_type] = any_regular_t(name_interface);
    else // if (type == eve_callback_suite_t::constant_k)
        cell[key_cell_type] = any_regular_t(name_constant);

    cell[key_name] = any_regular_t(name);
    cell[key_initializer] = any_regular_t(initializer);
    cell[key_comment_brief] = any_regular_t(brief);
    cell[key_comment_detailed] = any_regular_t(detailed);

    cell_set_m.push_back(cell);
}

/******************************************************************************/

struct layout_formatter_t
{
    typedef forest<dictionary_t> view_node_forest_t;
    typedef vector<dictionary_t> cell_node_set_t;

    explicit layout_formatter_t(std::ostream& out) :
        out_m(out),
        last_newline_m(0)
    { }

    ~layout_formatter_t()
    {
        out_m.flush();
    }

    void stream_out(const string_t&           layout_name,
                    const view_node_forest_t& view_node_forest,
                    const cell_node_set_t&    cell_node_set);

private:
    std::size_t indent() const
    {
        return static_cast<std::size_t>(out_m.tellp() - last_newline_m);
    }

    void newline()
    {
        out_m << '\n';

        last_newline_m = out_m.tellp();
    }

    void stream_out_parameter_set(const array_t& expression);

    void stream_out_cell_set(const cell_node_set_t& cell_node_set);
    
    std::ostream&  out_m;
    std::streampos last_newline_m;
};

/******************************************************************************/

void layout_formatter_t::stream_out_parameter_set(const array_t& expression)
{
    std::string str_expression(format_expression(expression, indent() - 2, true));

    if (!str_expression.empty())
    {
        std::string::iterator first(str_expression.begin());
        std::string::iterator last(--str_expression.end());
    
        while (std::isspace(*++first) && first != last);
        while (first != last && std::isspace(*last--));

        str_expression = std::string(first, last);
    }

    out_m << str_expression;
}

/******************************************************************************/

void layout_formatter_t::stream_out_cell_set(const cell_node_set_t& cell_node_set)
{
    name_t last_type;

    for (cell_node_set_t::const_iterator iter(cell_node_set.begin()),
         last(cell_node_set.end()); iter != last; ++iter)
    {
        name_t          type(get_value(*iter, key_cell_type).cast<name_t>());
        name_t          name(get_value(*iter, key_name).cast<name_t>());
        const string_t& brief(get_value(*iter, key_comment_brief).cast<string_t>());
        const string_t& detailed(get_value(*iter, key_comment_detailed).cast<string_t>());
        const array_t&  initializer(get_value(*iter, key_initializer).cast<array_t>());

        if (type != last_type)
        {
            last_type = type;

            newline();

            out_m << "  " << type.c_str() << ":";

            newline();
        }

        if (!detailed.empty())
        {
            out_m << adobe::spaces(4) << "/*" << detailed << "*/";

            newline();
        }

        std::string cell_name(name.c_str());

        std::stringstream header;

        header << adobe::spaces(4) << cell_name << ": ";

        std::size_t expr_indent(indent() + header.str().length());

        out_m << header.str() << format_expression(initializer, expr_indent, true) << ";";

        if (!brief.empty())
            out_m << " //" << brief;

        newline();
    }
}

/******************************************************************************/

void layout_formatter_t::stream_out(const string_t&           layout_name,
                                    const view_node_forest_t& view_node_forest,
                                    const cell_node_set_t&    cell_node_set)
{
    string_t washed_layout_name;

    for (string_t::const_iterator first(layout_name.begin()), last(layout_name.end()); first != last; ++first)
        washed_layout_name.push_back(std::isalpha(*first) ? *first : '_');

    out_m << "layout " << washed_layout_name.c_str();
    
    newline();

    out_m << "{";

    stream_out_cell_set(cell_node_set);

    newline();

    out_m << adobe::spaces(4) << "view";

    typedef depth_fullorder_iterator<boost::range_const_iterator<forest<dictionary_t> >::type> iterator;

    std::pair<iterator, iterator> range(depth_range(view_node_forest));

    for (iterator first(boost::begin(range)), last(boost::end(range)); first != last; ++first)
    {
        bool            is_leading(first.edge() == adobe::forest_leading_edge);
        name_t          name(get_value(*first, key_name).cast<name_t>());
        const array_t&  parameters(get_value(*first, key_parameters).cast<array_t>());
        const string_t& brief(get_value(*first, key_comment_brief).cast<string_t>());
        const string_t& detailed(get_value(*first, key_comment_detailed).cast<string_t>());
        std::size_t     indent((first.depth() + 1) * 4);

        if (is_leading)
        {
            if (!detailed.empty())
            {
                out_m << adobe::spaces(indent) << "/*" << detailed << "*/";
                newline();
            }

            if (first.depth())
                out_m << adobe::spaces(indent);
            else
                out_m << " ";

            out_m << name.c_str() << "(";

            stream_out_parameter_set(parameters);

            out_m << ")";

            if (has_children(first))
            {
                if (!brief.empty())
                    out_m << " //" << brief;

                newline();

                out_m << adobe::spaces(indent) << "{";

                newline();
            }
            else
            {
                out_m << ";";

                if (!brief.empty())
                    out_m << " //" << brief;

                newline();
            }
        }
        else
        {
            if (has_children(first))
            {
                out_m << adobe::spaces(indent) << "}";

                newline();
            }

            if (boost::next(first).edge() == adobe::forest_leading_edge)
                newline();
        }
    }

    out_m << "}";

    newline();
}

/******************************************************************************/

} // namespace

/******************************************************************************/

namespace adobe {

/******************************************************************************/

layout_assembly_t disassemble_layout(std::istream&          stream,
                                     const line_position_t& position)
{
    eve_node_forest_t parser(stream, position);

    return adobe::make_pair(parser.node_forest_m, parser.cell_set_m);
}

/******************************************************************************/

void assemble_layout(const string_t&          layout_name,
                     const layout_assembly_t& assembly,
                     std::ostream&            out)
{
    layout_formatter_t(out).stream_out(layout_name,
                                       assembly.first,
                                       assembly.second);
}

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
