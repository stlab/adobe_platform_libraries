/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/string.hpp>
#include <adobe/xstring.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <adobe/dictionary.hpp>

#include <adobe/file_slurp.hpp>

#include <iostream>
#include <sstream>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

inline std::string to_string(const adobe::token_range_t& x)
{ return std::string(x.first, x.second); }

/****************************************************************************************************/

} // namespace

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
extern "C" void tss_cleanup_implemented()
{ }
#endif

/****************************************************************************************************/

adobe::token_range_t sample_handler(const adobe::token_range_t&     entire_element_range,
                                    const adobe::token_range_t&     name,
                                    const adobe::attribute_set_t&   attribute_set,
                                    const adobe::token_range_t&     value)
{
    if (name == adobe::static_token_range("br"))
        return adobe::xml_element_linefeed(entire_element_range, name, attribute_set, value);
    else if (name == adobe::static_token_range("b"))
        // just for kicks: keep all bold tags found in the output
        return adobe::xml_element_echo(entire_element_range, name, attribute_set, value);
    else
        // if the tags aren't what we want, strip the tag
        return adobe::xml_element_strip(entire_element_range, name, attribute_set, value);
}

/****************************************************************************************************/

int main(int argc, char* argv[])
{
    int result(0);

    try
    {
        std::string glossary_name("../default.xstr");

        if (argc > 1) glossary_name.assign(argv[1]);

        bfs::path                   glossary(glossary_name.c_str(), argc > 1 ? bfs::native : bfs::portable_name);
        adobe::file_slurp<char>     glossary_slurp(glossary);
        adobe::xstring_context_t    context(glossary_slurp.begin(), glossary_slurp.end(),
                                            adobe::line_position_t(glossary_name.c_str()));

        glossary_slurp.release();

        context.set_preorder_predicate(adobe::always_true<adobe::token_range_t>());

        context.set_element_handler(sample_handler);

#if 1
        adobe::implementation::context_frame_t& context_frame(adobe::implementation::top_frame());

        std::cout   << "Parsed "
                    << static_cast<unsigned int>(context_frame.glossary_m.size())
                    << " elements:" << std::endl;

        typedef adobe::implementation::context_frame_t::store_t::iterator iterator;

        for (   iterator first(context_frame.glossary_m.begin()), last(context_frame.glossary_m.end());
                first != last; ++first)
            std::cout << "   " << to_string(first->second.second) << std::endl;
#endif

        std::string xstr(adobe::xstring("<xstr id='mixed_test'/>"));

        std::cout << xstr << std::endl;

        std::string xstr2;

        adobe::xstring("<xstr id='entity_ref_test'/>", std::back_inserter(xstr2));

        std::cout << xstr2 << std::endl;

        std::cout << adobe::xstring("<xstr id='markup_test_2'/>") << std::endl;

        std::cout << adobe::xstring("<xstr id='meganested_example'/>") << std::endl;

        std::cout << adobe::xstring("<xstr id='ok' lang='fr' platform='macintosh'/>") << std::endl;

        std::cout << "Inline xstring test:" << std::endl;

        std::cout << adobe::xstring("This is a test of the <xstr id='emergency'>emergency</xstr> broadcast <xstr id='system'>system</xstr>.") << std::endl;

        std::cout << "Nested xstring test:" << std::endl;

        std::cout << adobe::xstring("Nesting <xstr id='nesting_inside_0'>is a fun <xstr id='nesting_inside_1'>thing</xstr> to do</xstr>, don'chaknow") << std::endl;

        std::cout << "eacute test:" << std::endl;

        std::cout << adobe::xstring("<xstr id='eacute'>&eacute;</xstr>") << std::endl;

        std::cout << "Marker Replacement Tests:" << std::endl;

        std::string good_thing(adobe::xstring("<xstr id='a_good_thing'/>"));
        std::string bad_thing(adobe::xstring("<xstr id='a_bad_thing'/>"));
        std::string neutral_thing(adobe::xstring("<xstr id='a_neutral_thing'/>"));

        std::cout << "good marker: " << good_thing << std::endl;
        std::cout << "bad marker: " << bad_thing << std::endl;
        std::cout << "neutral marker: " << neutral_thing << std::endl;

        std::cout << "Good: " << adobe::xstring_replace(adobe::static_name_t("replacement_test_1"), good_thing) << std::endl;
        std::cout << "Bad: " << adobe::xstring_replace(adobe::static_name_t("replacement_test_1"), bad_thing) << std::endl;
        std::cout << "Neutral: " << adobe::xstring_replace(adobe::static_name_t("replacement_test_1"), neutral_thing) << std::endl;
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
