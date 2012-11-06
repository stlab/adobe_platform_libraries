/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_metrics.hpp>

#include <adobe/xstring.hpp>
#include <adobe/virtual_machine.hpp>
#include <adobe/implementation/expression_parser.hpp>
#include <adobe/name.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/array.hpp>
#include <adobe/once.hpp>

#include <sstream>

/****************************************************************************************************/

ADOBE_ONCE_DECLARATION(mac_metrics_once)

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const char* mac_metrics_g =
"<xstr id='metric:global'>{ gap: 4 }</xstr>"
""
"<xstr id='metric:window'>{ spacing: 20 }</xstr>"
""
"<xstr id='metric:checkbox' theme='normal'>"
"    { adjust_size: [ 2, 4 ], adjust_position: [ -2, -2 ], outset: [ 1, 2, 0, 1 ], adjust_baseline: -2 }</xstr>"
"<xstr id='metric:checkbox' theme='small'>"
"    { adjust_size: [ 1, 4 ], adjust_position: [ -1, -2 ], outset: [ 1, 2, 0, 1 ], adjust_baseline: -2 }</xstr>"
"<xstr id='metric:checkbox' theme='mini'>"
"    { adjust_size: [ 0, 1 ], adjust_position: [ 0, -1 ], outset: [ 0, 1, 0, 0 ], adjust_baseline: -1 }</xstr>"
""
"<xstr id='metric:radio_button' theme='normal'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ -2, -2 ], outset: [ 1, 0, 1, 1 ], adjust_baseline: -2 }</xstr>"
"<xstr id='metric:radio_button' theme='small'>"
"    { adjust_size: [ 0, 2 ], adjust_position: [ 0, -1 ], outset: [ 0, 0, 0, 1 ] }</xstr>"
"<xstr id='metric:radio_button' theme='mini'>"
"    { adjust_size: [ 0, 1 ], adjust_position: [ 0, 0 ], outset: [ 0, 0, 0, 1 ] }</xstr>"
""
"<xstr id='metric:button' theme='normal'>"
"    { adjust_size: [ 0, -1 ], outset: [ 1, 0, 1, 2 ] }</xstr>"
"<xstr id='metric:button' theme='small'>"
"    { adjust_size: [ -2, 2 ], adjust_position: [ 1, -1 ], outset: [ 0, 0, 0, 1 ], adjust_baseline: -2 }</xstr>"
"<xstr id='metric:button' theme='mini'>"
"    { adjust_size: [ 2, 5 ], adjust_position: [ -1, 0 ], outset: [ 1, 0, 1, 1 ], adjust_baseline: -3  }</xstr>"
""
"<xstr id='metric:group' label='true' theme='normal'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], frame: [ 1, 2, 1, 1 ], spacing: 20 }</xstr>"
"<xstr id='metric:group' label='true' theme='small'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], frame: [ 1, 2, 1, 1 ], spacing: 20 }</xstr>"
"<xstr id='metric:group' label='true' theme='mini'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], frame: [ 1, 3, 1, 1 ], spacing: 20 }</xstr>"
""
"<xstr id='metric:group' label='false' theme='normal'>"
"    { adjust_size: [ 0, 1 ], frame: [ 1, 4, 1, 1 ] }</xstr>"
"<xstr id='metric:group' label='false' theme='small'>"
"    { adjust_size: [ 0, 1 ], frame: [ 1, 2, 1, 1 ] }</xstr>"
"<xstr id='metric:group' label='false' theme='mini'>"
"    { adjust_size: [ 0, 1 ], frame: [ 1, 2, 1, 1 ] }</xstr>"
""
"<xstr id='metric:tab_group' theme='normal'>"
"    { adjust_size: [ 0, 8 ], adjust_position: [ 0, -7 ], frame: [ 1, 8, 1, 1 ], spacing: 24 }</xstr>"
"<xstr id='metric:tab_group' theme='small'>"
"    { adjust_size: [ 0, 5 ], adjust_position: [ 0, -4 ], frame: [ 1, 8, 1, 1 ], spacing: 19 }</xstr>"
"<xstr id='metric:tab_group' theme='mini'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], frame: [ 1, 6, 1, 1 ], spacing: 16 }</xstr>"
""
"<xstr id='metric:static_text' theme='normal'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -3 ], adjust_baseline: -3 }</xstr>"
"<xstr id='metric:static_text' theme='small'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], adjust_baseline: -2 }</xstr>"
"<xstr id='metric:static_text' theme='mini'>"
"    { adjust_size: [ 0, 3 ], adjust_position: [ 0, -2 ], adjust_baseline: -2 }</xstr>"
""
"<xstr id='metric:edit_text' scroll='false' theme='normal'>"
"    { adjust_size: [ -6, -6 ], adjust_position: [ 3, 3 ], outset: [ 3, 3, 3, 3 ], adjust_baseline: 3 }</xstr>"
"<xstr id='metric:edit_text' scroll='false' theme='small'>"
"    { adjust_size: [ -6, -6 ], adjust_position: [ 3, 3 ], outset: [ 3, 3, 3, 3 ], adjust_baseline: 3 }</xstr>"
"<xstr id='metric:edit_text' scroll='false' theme='mini'>"
"    { adjust_size: [ -6, -6 ], adjust_position: [ 3, 3 ], outset: [ 3, 3, 3, 3 ], adjust_baseline: 3 }</xstr>"
""
"<xstr id='metric:edit_text' scroll='true' theme='normal'>"
"    { }</xstr>"
"<xstr id='metric:edit_text' scroll='true' theme='small'>"
"    { }</xstr>"
"<xstr id='metric:edit_text' scroll='true' theme='mini'>"
"    { }</xstr>"
""
"<xstr id='metric:popup' label='false' theme='normal'>"
"    { adjust_size: [ 1, -1 ], adjust_position: [ -1, 0 ], outset: [ 1, 0, 1, 1 ] }</xstr>"
"<xstr id='metric:popup' label='false' theme='small'>"
"    { adjust_size: [ 1, -1 ], adjust_position: [ 1, 0 ], outset: [ 1, 0, 1, 1 ], adjust_baseline: 1 }</xstr>"
"<xstr id='metric:popup' label='false' theme='mini'>"
"    { adjust_size: [ 2, 0 ], adjust_position: [ -1, 0 ] }</xstr>"
""
"<xstr id='metric:popup' label='true' theme='normal'>"
"    { adjust_size: [ 0, -1 ], outset: [ 0, 0, 1, 1 ] }</xstr>"
"<xstr id='metric:popup' label='true' theme='small'>"
"    { adjust_size: [ 1, -1 ], adjust_position: [ 0, 0 ], outset: [ 0, 0, 1, 1 ], adjust_baseline: 1 }</xstr>"
"<xstr id='metric:popup' label='true' theme='mini'>"
"    { adjust_size: [ 1, 0 ] }</xstr>"
;

/****************************************************************************************************/

ADOBE_THREAD_LOCAL_STORAGE(adobe::virtual_machine_t, metrics_vm)

/*************************************************************************************************/

void init_mac_metrics_once()
{
    // initialize the thread-specific virtual machine

    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(metrics_vm);

    // initialize the mac-specific metrics glossary
    // we have to make a copy of the buffer because the parsing context takes ownership of it.

    std::size_t metrics_len(std::strlen(mac_metrics_g));
    char*       metrics_data(new char[metrics_len + 1]);

    std::copy(&mac_metrics_g[0], &mac_metrics_g[0] + metrics_len, &metrics_data[0]);

    metrics_data[metrics_len] = 0;

    static adobe::xstring_context_t mac_metrics_context(&metrics_data[0], &metrics_data[metrics_len],
                                                        adobe::line_position_t( "mac metrics" ) );
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

dictionary_t widget_metrics(const std::string& xstr)
{ return widget_metrics(xstr, dictionary_t()); }

dictionary_t widget_metrics(const std::string& xstr, const dictionary_t& context)
{
    ADOBE_ONCE_INSTANCE(mac_metrics_once);

    std::vector<attribute_set_t::value_type> context_attribute_set;

    if (!context.count(static_name_t("theme")))
        context_attribute_set.push_back(std::make_pair(static_token_range("theme"), static_token_range("normal")));

    for (   dictionary_t::const_iterator first(context.begin()), last(context.end());
            first != last; ++first)
    {
        const char* key_c_str(first->first.c_str());
        const char* value_c_str(first->second.cast<name_t>().c_str());
        uchar_ptr_t key_uc_str(reinterpret_cast<uchar_ptr_t>(key_c_str));
        uchar_ptr_t value_uc_str(reinterpret_cast<uchar_ptr_t>(value_c_str));

        token_range_t key(key_uc_str, key_uc_str + std::strlen(key_c_str));
        token_range_t value(value_uc_str, value_uc_str + std::strlen(value_c_str));

        context_attribute_set.push_back(std::make_pair(key, value));
    }

    xstring_context_t local_context(context_attribute_set.begin(), context_attribute_set.end());

    std::string utf8(xstring(xstr.c_str(), xstr.size()));

    if (utf8.empty()) return dictionary_t();

    std::stringstream   expression_stream(utf8);
    expression_parser   parser(expression_stream, line_position_t("widget_metrics"));
    array_t             expression;

    parser.require_expression(expression);

    ADOBE_THREAD_LOCAL_STORAGE_ACCESS(metrics_vm).evaluate(expression);

    dictionary_t result(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(metrics_vm).back().cast<dictionary_t>());

    ADOBE_THREAD_LOCAL_STORAGE_ACCESS(metrics_vm).pop_back();

    return result;
}

/****************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(mac_metrics_once, init_mac_metrics_once)

/****************************************************************************************************/
