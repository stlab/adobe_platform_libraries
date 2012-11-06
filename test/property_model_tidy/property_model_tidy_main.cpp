/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <fstream>
#include <iostream>

#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/property_model_formatter.hpp>

/******************************************************************************/

namespace {

/******************************************************************************/

std::string get_line(std::istream&  stream,
                     std::streampos line_start_position)
{
    if (line_start_position != std::streampos(0))
        line_start_position = static_cast<std::streamoff>(static_cast<std::size_t>(line_start_position) - 1);

    stream.seekg(line_start_position);

    adobe::vector<char> buffer(512, 0);

    stream.getline(&buffer[0], static_cast<std::streamsize>(buffer.size()));

    return std::string(&buffer[0], stream.gcount());
}

/******************************************************************************/

void usage()
{
    std::cerr << "Property Model Tidy v" << ADOBE_VERSION_MAJOR << '.'
              << ADOBE_VERSION_MINOR << '.' << ADOBE_VERSION_SUBMINOR << std::endl
              << "Usage: property_model_tidy [-d] source_file" << std::endl
              << "Specify -d to disassemble only." << std::endl;

    throw std::runtime_error("parameter error");
}

/******************************************************************************/

} // namespace

/******************************************************************************/

int main(int argc, char** argv)
try
{
    if (argc < 2)
        usage();

    bool          disassemble_only(std::strcmp(argv[1], "-d") == 0);
    const char*   filename(argv[disassemble_only ? 2 : 1]);
    std::ifstream input(filename);

    if (!input.is_open())
        throw std::runtime_error(adobe::make_string("Input file ",
                                                    filename,
                                                    " could not be opened for read."));

    try
    {
        adobe::line_position_t::getline_proc_t getline_proc(
            new adobe::line_position_t::getline_proc_impl_t(boost::bind(&get_line,
                                                                        boost::ref(input),
                                                                        _2)));

        adobe::sheet_assembly_t assy(adobe::disassemble_sheet(input,
                                                       adobe::line_position_t(adobe::name_t(filename),
                                                                              getline_proc)));

        if (disassemble_only)
            std::cout << adobe::begin_asl_cel << assy << adobe::end_asl_cel;
        else
            adobe::assemble_sheet(filename, assy, std::cout);
    }
    catch (const adobe::stream_error_t& error)
    {
        std::cerr << "Exception: " << format_stream_error(input, error);

        return 1;
    }

    return 0;
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return 1;
}
catch (...)
{
    std::cerr << "Exception: unknown" << std::endl;

    return 1;
}

/******************************************************************************/
