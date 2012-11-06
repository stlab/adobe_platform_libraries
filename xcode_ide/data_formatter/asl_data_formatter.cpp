/*
    Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#import "/Developer/Applications/Xcode.app/Contents/PlugIns/GDBMIDebugging.xcplugin/Contents/Headers/DataFormatterPlugin.h"

/******************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <Carbon/Carbon.h>

#define ADOBE_LOGGING        0
#define ADOBE_CDF_USES_LOCKS 1

#if ADOBE_CDF_USES_LOCKS
    #include <boost/thread/mutex.hpp>
    #include <boost/thread/thread.hpp>
#endif

#define ADOBE_STD_SERIALIZATION 1

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/string.hpp>

/******************************************************************************/

#define ADOBE_EXPORT __attribute__((visibility("default")))

/******************************************************************************/

extern "C" {

/******************************************************************************/

//  A bundle to support custom data formatting must define the following symbol
ADOBE_EXPORT _pbxgdb_plugin_function_list* _pbxgdb_plugin_functions;

/******************************************************************************/

} // extern "C"

/******************************************************************************/

namespace {

/******************************************************************************/

static const char* error_plugin_hooks_null_k("asl_data_formatter error: plugin hooks not installed");
static const char* error_message_hooks_null_k("asl_data_formatter error: message hook not installed");

/******************************************************************************/
#if ADOBE_LOGGING
std::ostream& log()
{
#if 0
    static std::ofstream log_s("asl_data_formatter_error_log.txt");
    static bool          inited_s(false);

    if (!inited_s)
    {
        inited_s = true;

        log_s << "**** LOGGING SESSION STARTED ****" << std::endl;
    }

    return log_s;
#else
    return std::cerr;
#endif
}
#endif
/******************************************************************************/

adobe::string_t postprocess_formatted(const std::string& formatted)
{
    /*
        Postprocessing to convert newlines, tabs, etc. to spaces to
        make things easier to read in the debugger's variable area
    */

    std::vector<char> formatted_string(formatted.size() + 1, 0);
    char*             dst(&formatted_string[0]);
    char              last_char(0);

    for (std::string::const_iterator iter(formatted.begin()),
         last(formatted.end()); iter != last; ++iter)
    {
        char insert(std::isspace(*iter) ? ' ' : *iter);

        if (insert == ' ' && last_char == ' ')
            continue;

        *dst++ = insert;
        last_char = insert;
    }

    return adobe::string_t(&formatted_string[0]);
}

/******************************************************************************/

inline const char* make_result_string(int identifier, const std::string& input_string)
{
    return _pbxgdb_plugin_functions->message(identifier,
                                             "%s",
                                             postprocess_formatted(input_string).c_str());
}

/******************************************************************************/

template <typename T>
const char* data_format(const T& value, int identifier)
{
#if ADOBE_CDF_USES_LOCKS
    // The iostreams are not guaranteed to be thread-safe!
    static boost::try_mutex           mutex_s;
    boost::try_mutex::scoped_try_lock scoped_lock(mutex_s);
#endif

    std::stringstream formatted;

    formatted << adobe::begin_asl_cel << value << adobe::end_asl_cel;

#if ADOBE_LOGGING
    log() << "<data_format 'type'='" << typeid(T).name() << "'>\n\tresult:" << formatted.str() << "\n</data_format>" << std::endl;
#endif

    return make_result_string(identifier, formatted.str());
}

/******************************************************************************/

template <> const char* data_format(const adobe::string_t& value, int identifier);
template <> const char* data_format(const adobe::array_t& value, int identifier);
template <> const char* data_format(const adobe::dictionary_t& value, int identifier);
template <> const char* data_format(const adobe::any_regular_t& value, int identifier);

/******************************************************************************/

template <>
const char* data_format(const adobe::string_t& value, int identifier)
{
#if ADOBE_LOGGING
    log() << "<data_format 'type'='string_t'>\n\tresult:" << value.c_str() << "\n</data_format>" << std::endl;
#endif

    return make_result_string(identifier, value);
}

/******************************************************************************/

template <>
const char* data_format(const adobe::array_t& value, int identifier)
{
    std::string result("[ ");

    for (adobe::array_t::const_iterator iter(value.begin()), last(value.end());
         iter != last; ++iter)
    {
        if (iter != value.begin())
            result << ", ";

        result << data_format(*iter, identifier);
    }

    result << " ]";

#if ADOBE_LOGGING
    log() << "<data_format 'type'='array_t'>\n\tresult:" << result << "\n</data_format>" << std::endl;
#endif

    return make_result_string(identifier, result);
}

/******************************************************************************/

template <>
const char* data_format(const adobe::dictionary_t& value, int identifier)
{
    std::string result("{ ");

    for (adobe::dictionary_t::const_iterator iter(value.begin()),
         last(value.end()); iter != last; ++iter)
    {
        if (iter != value.begin())
            result << ", ";

        result << iter->first.c_str() << ": " << data_format(iter->second, identifier);
    }

    result << " }";

#if ADOBE_LOGGING
    log() << "<data_format 'type'='dictionary_t'>\n\tresult:" << result << "\n</data_format>" << std::endl;
#endif

    return make_result_string(identifier, result);
}

/******************************************************************************/

template <typename T>
const char* log_and_format(const adobe::any_regular_t& value, int identifier)
{
    const char* result(data_format(value.cast<T>(), identifier));

#if ADOBE_LOGGING
    log() << "<data_format 'type'='any_regular_t:" << value.type_info().name() << "'>\n\tresult:" << result << "\n</data_format>" << std::endl;
#endif

    return result;
}

/******************************************************************************/

template <>
const char* data_format(const adobe::any_regular_t& value, int identifier)
{
    const adobe::type_info_t& value_type_info(value.type_info());

    // This typeinfo checklist *MUST* match any_regular.cpp's
    // set of types it knows how to serialize, otherwise the
    // plugin will fire asserts at the user while they're using
    // it, which is a Bad Thing.

    if (value_type_info == adobe::type_info<double>())                   return log_and_format<double>(value, identifier);
    else if (value_type_info == adobe::type_info<bool>())                return log_and_format<bool>(value, identifier);
    else if (value_type_info == adobe::type_info<adobe::empty_t>())      return log_and_format<adobe::empty_t>(value, identifier);
    else if (value_type_info == adobe::type_info<adobe::string_t>())     return log_and_format<adobe::string_t>(value, identifier);
    else if (value_type_info == adobe::type_info<adobe::name_t>())       return log_and_format<adobe::name_t>(value, identifier);
    else if (value_type_info == adobe::type_info<adobe::array_t>())      return log_and_format<adobe::array_t>(value, identifier);
    else if (value_type_info == adobe::type_info<adobe::dictionary_t>()) return log_and_format<adobe::dictionary_t>(value, identifier);
    else if (value_type_info == adobe::type_info<std::size_t>())         return log_and_format<std::size_t>(value, identifier);

    // If we've gotten to this point we're dealing with a type we
    // cannot serialize, so we'll have to summarize it.

    return make_result_string(identifier, value_type_info.name());
}

/******************************************************************************/

} // namespace

/******************************************************************************/

#define ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(type) \
ADOBE_EXPORT const char* format_##type(adobe::type* value, int identifier) \
{ \
    if (_pbxgdb_plugin_functions == 0) return error_plugin_hooks_null_k; \
    if (_pbxgdb_plugin_functions->message == 0) return error_message_hooks_null_k; \
    if (value == 0) return "<null pointer>"; \
    try \
    { \
        return data_format(*value, identifier); \
    } \
    catch (const std::exception& error) \
    { \
        std::cerr << "asl_data_formatter format_"#type" exception: " << error.what() << std::endl; \
    } \
    catch (...) \
    { \
        std::cerr << "asl_data_formatter format_"#type" exception: unknown" << std::endl; \
    } \
    return "format_"#type" exception caught; check stderr for details"; \
}

/******************************************************************************/

extern "C" {

/******************************************************************************/

ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(any_regular_t)
ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(dictionary_t)
ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(array_t)
ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(name_t)
ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(string_t)
ADOBE_CUSTOM_DATA_FORMATTER_SERIALIZATION_BOILERPLATE(empty_t)

/******************************************************************************/

} // extern "C"

/******************************************************************************/

