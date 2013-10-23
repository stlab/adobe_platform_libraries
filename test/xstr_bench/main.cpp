/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/xstring.hpp>

#include <adobe/timer.hpp>
#include <adobe/file_slurp.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
extern "C" void tss_cleanup_implemented()
{ }
#endif

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const char* glossary_name_g("glossary.xstr");

/****************************************************************************************************/

const char* lorem_g[] =
{
"a", "ac", "accumsan", "ad", "adipiscing", "Aenean", "aliquam",
"Aliquam", "aliquet", "amet", "ante", "aptent", "arcu",
"at", "auctor", "augue", "bibendum", "blandit", "Class",
"commodo", "condimentum", "congue", "consectetuer", "consequat", "conubia",
"convallis", "Cras", "Cum", "Curabitur", "cursus", "dapibus",
"diam", "dictum", "dictumst", "dignissim", "dis", "dolor",
"Donec", "dui", "Duis", "egestas", "eget", "elementum",
"elit", "enim", "erat", "eros", "est", "et",
"Etiam", "eu", "facilisi", "facilisis", "felis", "fermentum",
"feugiat", "fringilla", "Fusce", "gravida", "habitasse", "hac",
"hendrerit", "hymenaeos", "iaculis", "id", "imperdiet", "in",
"In", "inceptos", "Integer", "interdum", "ipsum", "justo",
"lacus", "laoreet", "lectus", "leo", "libero", "ligula",
"litora", "lobortis", "lorem", "Lorem", "luctus", "Maecenas",
"magna", "magnis", "massa", "mattis", "mauris", "Mauris",
"metus", "mi", "mollis", "montes", "Morbi", "mus",
"Nam", "nascetur", "natoque", "nec", "neque", "nibh",
"nisl", "non", "nonummy", "nostra", "nulla", "Nulla",
"Nullam", "nunc", "Nunc", "odio", "orci", "ornare",
"parturient", "pede", "pellentesque", "Pellentesque", "penatibus", "per",
"pharetra", "Phasellus", "placerat", "platea", "porta", "porttitor",
"Praesent", "pretium", "Proin", "purus", "quam", "quis",
"Quisque", "rhoncus", "ridiculus", "risus", "rutrum", "sagittis",
"sapien", "scelerisque", "sed", "Sed", "sem", "sit",
"sociis", "sociosqu", "sodales", "sollicitudin", "suscipit", "Suspendisse",
"taciti", "tellus", "tempor", "tincidunt", "torquent", "tortor",
"tristique", "turpis", "ullamcorper", "ultrices", "ultricies", "urna",
"ut", "Ut", "vehicula", "vel", "velit", "venenatis",
"vestibulum", "Vestibulum", "vitae", "Vivamus", "volutpat", "vulputate",
"wisi"
};

std::size_t lorem_size_g(sizeof(lorem_g) / sizeof(lorem_g[0]));

/****************************************************************************************************/

std::string generate_lorem(int min, int max, bool nopunct = false)
{
    std::string         result;
    std::size_t         this_count(std::rand() % (max - min + 1) + min);

    for (std::size_t i(0); i < this_count; ++i)
    {
        if (i != 0) result << " ";

        result << lorem_g[std::rand() % lorem_size_g];
    }

    if (nopunct)
        for (std::string::iterator first(result.begin()), last(result.end()); first != last; ++first)
            if (!std::isalpha(*first))
                *first = '_';

    return result;
}

/****************************************************************************************************/

void generate_file(int argc, char** argv)
{
    std::size_t test_string_count(15000);

    if (argc > 1)
        test_string_count = std::atoi(argv[1]);

    boost::filesystem::path boost_path(glossary_name_g, boost::filesystem::native);

    std::cout << "Generating xstring file with " << test_string_count << " strings..." << std::endl;

    boost::filesystem::ofstream out(boost_path);

    adobe::timer_t gen_timer;

    for (std::size_t i(0); i < test_string_count; ++i)
        out << "<xstr id='" << generate_lorem(2, 5, true) << "-" << i << "'>" << generate_lorem(1, 30) << "</xstr>" << std::endl;

    gen_timer.report("Generation");

    out.close();
}

/****************************************************************************************************/

std::size_t do_test(adobe::file_slurp<char>& slurp, adobe::timer_t& timer)
{
    slurp.reslurp();

    timer.reset();

    adobe::xstring_context_t context(   slurp.begin(), slurp.end(),
                                        adobe::line_position_t(glossary_name_g));

    timer.accrue();

    slurp.release();

    return adobe::implementation::top_frame().glossary_m.size();
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main(int argc, char** argv)
{
    int result(0);

    std::srand(std::time(NULL));

    try
    {
        boost::filesystem::path boost_path(glossary_name_g, boost::filesystem::native);

        if (!boost::filesystem::exists(boost_path))
            generate_file(argc, argv);

        adobe::file_slurp<char> slurp(boost_path);
        adobe::timer_t          timer;
        std::size_t             repeat_count(20);
        std::size_t             count(0);

        std::cout << "Repeating test " << repeat_count << " times..." << std::endl;

        for (std::size_t i(0); i < repeat_count;)
        {
            count = std::max(count, do_test(slurp, timer));

            std::cerr << ++i << " ";
        }

        std::cout << std::endl;

        double avg(timer.accrued_average());

        std::cout << "Average time taken: " << avg << " milliseconds (" << avg / 1e3 << " sec)" << std::endl;

        std::cout << "Found " << count << " parsed elements in glossary." << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Exception: " << error.what() << "\n";

        result = 1;
    }
    catch (...)
    {
        std::cerr << "Unknown Exception\n";

        result = 1;
    }

    return result;
}
