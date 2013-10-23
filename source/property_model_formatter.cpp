/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <adobe/config.hpp>

#include <cctype>
#include <sstream>

#include <adobe/adam_parser.hpp>
#include <adobe/algorithm.hpp>
#include <adobe/formatter_tokens.hpp>
#include <adobe/implementation/expression_formatter.hpp>
#include <adobe/property_model_formatter.hpp>

/******************************************************************************/

namespace {

/******************************************************************************/

using namespace adobe;

/******************************************************************************/

const aggregate_name_t cell_type_constant  = { "constant" };
const aggregate_name_t cell_type_input     = { "input" };
const aggregate_name_t cell_type_interface = { "interface" };
const aggregate_name_t cell_type_invariant = { "invariant" };
const aggregate_name_t cell_type_logic     = { "logic" };
const aggregate_name_t cell_type_output    = { "output" };

/******************************************************************************/

// TODO : Must be updated for external cells

struct adam_node_parse_engine_t
{
public:
    adam_node_parse_engine_t(std::istream& adam_stream, const line_position_t& line_position)
    {
        suite_m.add_cell_proc_m = boost::bind(&adam_node_parse_engine_t::add_cell,
                                              boost::ref(*this), _1, _2, _3, _4, _5, _6);
        suite_m.add_relation_proc_m = boost::bind(&adam_node_parse_engine_t::add_relation,
                                              boost::ref(*this), _1, _2, _3, _4, _5, _6);
        suite_m.add_interface_proc_m = boost::bind(&adam_node_parse_engine_t::add_interface,
                                              boost::ref(*this), _1, _2, _3, _4, _5, _6, _7, _8);

        parse(adam_stream, line_position, suite_m);
    }

    sheet_assembly_t result_m;

private:
    typedef adam_callback_suite_t::relation_t  relation_t;
    typedef adam_callback_suite_t::cell_type_t cell_type_t;

    void add_cell(cell_type_t            type,
                  name_t                 cell_name,
                  const line_position_t& position,
                  const array_t&         expr_or_init,
                  const std::string&     brief,
                  const std::string&     detailed);

    void add_relation(const line_position_t& position,
                      const array_t&         conditional,
                      const relation_t*      first,
                      const relation_t*      last,
                      const std::string&     brief,
                      const std::string&     detailed);
    
    void add_interface(name_t                 cell_name,
                       bool                   linked,
                       const line_position_t& position1,
                       const array_t&         initializer,
                       const line_position_t& position2,
                       const array_t&         expression,
                       const std::string&     brief,
                       const std::string&     detailed);

    array_t make_relation_set(const relation_t* first,
                              const relation_t* last);

    adam_callback_suite_t suite_m;
};

/******************************************************************************/

void adam_node_parse_engine_t::add_cell(cell_type_t            type,
                                        name_t                 cell_name,
                                        const line_position_t& /*position*/,
                                        const array_t&         expr_or_init,
                                        const std::string&     brief,
                                        const std::string&     detailed)
{
    dictionary_t node;

    node[key_cell_meta_type] = any_regular_t(key_meta_type_cell);

    if (type == adam_callback_suite_t::input_k)
        node[key_cell_type] = any_regular_t(cell_type_input);
    else if (type == adam_callback_suite_t::output_k)
        node[key_cell_type] = any_regular_t(cell_type_output);
    else if (type == adam_callback_suite_t::constant_k)
        node[key_cell_type] = any_regular_t(cell_type_constant);
    else if (type == adam_callback_suite_t::logic_k)
        node[key_cell_type] = any_regular_t(cell_type_logic);
    else // if (type == adam_callback_suite_t::invariant_k)
        node[key_cell_type] = any_regular_t(cell_type_invariant);

    node[key_name] = any_regular_t(cell_name);

    if (type == adam_callback_suite_t::input_k ||
        type == adam_callback_suite_t::constant_k)
        node[key_initializer] = any_regular_t(expr_or_init);
    else
        node[key_expression] = any_regular_t(expr_or_init);
        
    node[key_comment_brief] = any_regular_t(brief);
    node[key_comment_detailed] = any_regular_t(detailed);

    result_m.push_back(node);
}

/******************************************************************************/

array_t adam_node_parse_engine_t::make_relation_set(const relation_t* first,
                                                    const relation_t* last)
{
    array_t relation_set;

    for (; first != last; ++first)
    {
        dictionary_t relation;

        relation[key_name_set] = any_regular_t(first->name_set_m);
        relation[key_expression] = any_regular_t(first->expression_m);
        relation[key_comment_brief] = any_regular_t(first->brief_m);
        relation[key_comment_detailed] = any_regular_t(first->detailed_m);

        relation_set.push_back(any_regular_t(relation));
    }

    return relation_set;
}

/******************************************************************************/

void adam_node_parse_engine_t::add_relation(const line_position_t& /*position*/,
                                            const array_t&         conditional,
                                            const relation_t*      first,
                                            const relation_t*      last,
                                            const std::string&     brief,
                                            const std::string&     detailed)
{
    dictionary_t node;

    node[key_cell_meta_type] = any_regular_t(key_meta_type_relation);

    node[key_conditional] = any_regular_t(conditional);
    node[key_relation_set] = any_regular_t(make_relation_set(first, last));
    node[key_comment_brief] = any_regular_t(brief);
    node[key_comment_detailed] = any_regular_t(detailed);

    result_m.push_back(node);
}

/******************************************************************************/

void adam_node_parse_engine_t::add_interface(name_t                 cell_name,
                                             bool                   linked,
                                             const line_position_t& /*position1*/,
                                             const array_t&         initializer,
                                             const line_position_t& /*position2*/,
                                             const array_t&         expression,
                                             const std::string&     brief,
                                             const std::string&     detailed)
{
    dictionary_t node;

    node[key_cell_meta_type] = any_regular_t(key_meta_type_interface);

    node[key_name] = any_regular_t(cell_name);
    node[key_linked] = any_regular_t(linked);
    node[key_initializer] = any_regular_t(initializer);
    node[key_expression] = any_regular_t(expression);
    node[key_comment_brief] = any_regular_t(brief);
    node[key_comment_detailed] = any_regular_t(detailed);

    result_m.push_back(node);
}

/******************************************************************************/

struct adam_node_formatter_t
{
private:
    typedef adam_callback_suite_t::cell_type_t cell_type_t;

public:
    void format_node(const dictionary_t& node, std::ostream& out);

private:
    void format_cell_node(const dictionary_t& node, std::ostream& out);
    void format_relation(const any_regular_t& any_relation, std::ostream& out);
    void format_relation_node(const dictionary_t& node, std::ostream& out);
    void format_interface_node(const dictionary_t& node, std::ostream& out);

    name_t last_meta_cell_type_m;
    name_t last_cell_type_m;
};

/******************************************************************************/

void adam_node_formatter_t::format_node(const dictionary_t& node, std::ostream& out)
{
    name_t meta_type(get_value(node, key_cell_meta_type).cast<name_t>());

    if (meta_type == key_meta_type_cell)
        format_cell_node(node, out);
    else if (meta_type == key_meta_type_relation)
        format_relation_node(node, out);
    else /* if (meta_type == key_meta_type_interface) */
        format_interface_node(node, out);
}

/******************************************************************************/

void adam_node_formatter_t::format_cell_node(const dictionary_t& node,
                                             std::ostream&       out)
{
    name_t cell_type(get_value(node, key_cell_type).cast<name_t>());

    if (last_meta_cell_type_m != key_meta_type_cell || last_cell_type_m != cell_type)
    {
        last_meta_cell_type_m = key_meta_type_cell;
        last_cell_type_m = cell_type;

        out << "\n  " << cell_type.c_str() << ":" << std::endl;
    }

    std::stringstream header;

    header << adobe::spaces(4) << get_value(node, key_name).cast<name_t>();

    out << header.str();

    if (cell_type == cell_type_input ||
        cell_type == cell_type_constant)
    {
        std::string initializer(format_expression(get_value(node, key_initializer).cast<array_t>(), header.str().size() + 2, true));

        out << ": " << initializer;
    }
    else /* if (cell_type == cell_type_output ||
                cell_type == cell_type_logic ||
                cell_type == cell_type_invariant) */
    {
        std::string definition(format_expression(get_value(node, key_expression).cast<array_t>(), header.str().size() + 5, true));

        out << " <== " << definition;
    }

    out << ";";

    const string_t& comment_brief(get_value(node, key_comment_brief).cast<string_t>());

    if (!comment_brief.empty())
        out << " //" << comment_brief;

    out << std::endl;
}

/******************************************************************************/

void adam_node_formatter_t::format_relation(const any_regular_t& any_relation,
                                            std::ostream&        out)
{
    typedef vector<name_t> name_set_t;

    const dictionary_t& relation(any_relation.cast<dictionary_t>());
    const string_t&     comment_detailed(get_value(relation, key_comment_detailed).cast<string_t>());

    if (!comment_detailed.empty())
        out << adobe::spaces(8) << "/*" << comment_detailed << "*/" << std::endl;

    name_set_t  name_set(get_value(relation, key_name_set).cast<name_set_t>());
    std::size_t name_set_count(name_set.size());

    out << adobe::spaces(8);

    if (name_set_count == 1)
    {
        out << name_set[0];
    }
    else
    {
        out << '[';
        for (std::size_t i(0); i < name_set_count; ++i)
        {
            if (i != 0)
                out << ", ";

            out << name_set[i];
        }
        out << ']';
    }
    
    out << " <== "
        << format_expression(get_value(relation, key_expression).cast<array_t>(), 8)
        << ";";

    const string_t& comment_brief(get_value(relation, key_comment_brief).cast<string_t>());

    if (!comment_brief.empty())
        out << " //" << comment_brief;

    out << std::endl;
}

/******************************************************************************/

void adam_node_formatter_t::format_relation_node(const dictionary_t& node,
                                                 std::ostream&       out)
{
    const array_t& conditional(get_value(node, key_conditional).cast<array_t>());

    if (last_meta_cell_type_m != key_meta_type_relation)
    {
        last_meta_cell_type_m = key_meta_type_relation;

        out << "\n  logic:";
    }

    const string_t& comment_detailed(get_value(node, key_comment_detailed).cast<string_t>());

    if (!comment_detailed.empty())
        out << "\n" << adobe::spaces(4) << "/*" << comment_detailed << "*/";

    out << "\n" << adobe::spaces(4);

    if (!conditional.empty())
        out << "when (" << format_expression(conditional) << ") ";

    out << "relate\n" << adobe::spaces(4) << "{\n";

    for_each(get_value(node, key_relation_set).cast<array_t>(),
             boost::bind(&adam_node_formatter_t::format_relation,
                         boost::ref(*this), _1, boost::ref(out)));

    out << adobe::spaces(4) << "}";

    const string_t& comment_brief(get_value(node, key_comment_brief).cast<string_t>());

    if (!comment_brief.empty())
        out << " //" << comment_brief;

    out << std::endl;
}

/******************************************************************************/

void adam_node_formatter_t::format_interface_node(const dictionary_t& node,
                                                  std::ostream&       out)
{
    if (last_meta_cell_type_m != key_meta_type_interface)
    {
        last_meta_cell_type_m = key_meta_type_interface;

        out << "\n  interface:" << std::endl;
    }

    const string_t& comment_detailed(get_value(node, key_comment_detailed).cast<string_t>());

    if (!comment_detailed.empty())
        out << "    /*" << comment_detailed << "*/" << std::endl;

    std::stringstream header;

    header << adobe::spaces(4)
           << (get_value(node, key_linked).cast<bool>() ? "" : "unlink ")
           << get_value(node, key_name).cast<name_t>();

    out << header.str();

    const array_t& initializer(get_value(node, key_initializer).cast<array_t>());
    const array_t& expression(get_value(node, key_expression).cast<array_t>());

    if (!initializer.empty())
        out << ": " << format_expression(initializer, header.str().size() + 2, true);

    if (!expression.empty())
        out << " <== " << format_expression(expression, header.str().size() + (!initializer.empty() ? 6 : 2), true);

    out << ";";

    const string_t& comment_brief(get_value(node, key_comment_brief).cast<string_t>());

    if (!comment_brief.empty())
        out << " //" << comment_brief;

    out << std::endl;
}

/******************************************************************************/

} // namespace

/******************************************************************************/

namespace adobe {

/******************************************************************************/

sheet_assembly_t disassemble_sheet(std::istream&          stream,
                                   const line_position_t& position)
{
    return adam_node_parse_engine_t(stream, position).result_m;
}

/******************************************************************************/

void assemble_sheet(const string_t&         sheet_name,
                    const sheet_assembly_t& assembly,
                    std::ostream&           out)
{
    string_t              washed_sheet_name;
    adam_node_formatter_t formatter;

    for (string_t::const_iterator first(sheet_name.begin()), last(sheet_name.end()); first != last; ++first)
        washed_sheet_name.push_back(std::isalpha(*first) ? *first : '_');

    out << "sheet " << washed_sheet_name.c_str() << "\n{";

    for_each(assembly, boost::bind(&adam_node_formatter_t::format_node,
                                   boost::ref(formatter), _1, boost::ref(out)));

    out << "}" << std::endl;
}

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
