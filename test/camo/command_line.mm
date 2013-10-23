/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include "command_line.hpp"

#include <stdexcept>

#include <adobe/future/macintosh_cocoa_bridge.hpp>
#include <adobe/string.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

task_result_t command_line(const char* command, const arg_set_t& arg_set)
try
{
    adobe::objc_auto_release_pool_t pool;
    std::string                     result;
    int                             status(-1);

    NS_DURING
        NSArray*      argument_set = [ [ NSArray alloc ] init ];
        NSTask*       the_task = [ [ NSTask alloc ] init ];
        NSPipe*       output_pipe = [ NSPipe pipe ];
        NSFileHandle* read_handle = [ output_pipe fileHandleForReading ];

        for (arg_set_t::const_iterator first(arg_set.begin()), last(arg_set.end()); first != last; ++first)
            argument_set = [ argument_set arrayByAddingObject:[ NSString stringWithCString:first->c_str() ] ];

        [ the_task setLaunchPath:[ NSString stringWithCString:command ] ];
        [ the_task setArguments:argument_set ];
        [ the_task setStandardOutput:output_pipe ];
        [ the_task setStandardError:output_pipe ];
        [ the_task launch ];
        [ the_task waitUntilExit ];

        NSString* task_output_string = [ [ NSString alloc ] initWithData:[ read_handle readDataToEndOfFile ]
                                                            encoding:NSASCIIStringEncoding ];

        result = std::string([ task_output_string UTF8String ]);
        status = [ the_task terminationStatus ];
    NS_HANDLER
        const char* what = [ [ localException reason ] UTF8String ];

        throw std::runtime_error(adobe::make_string("Obj-C: ", what,
                                                    adobe::make_string(" (", command, ")").c_str()));
    NS_ENDHANDLER

    return task_result_t(status, result);
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return task_result_t();
}
catch(...)
{
    std::cerr << "Exception: unknown" << std::endl;

    return task_result_t();
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
