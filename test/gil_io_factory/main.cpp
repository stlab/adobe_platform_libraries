/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <iterator>

#include <boost/gil/extension/io/io_factory.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/core/image.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>
#include <cassert>

/*************************************************************************************************/

int main(int argc, char* argv[])
try
{
    typedef GIL::rgba8_view                 view_type;
    typedef GIL::image_factory_t<view_type> factory_type;
    typedef factory_type::image_format_type format_type;

    factory_type            factory;
    boost::filesystem::path test_src(argc > 1 ? argv[1] : "", boost::filesystem::native);
    adobe::static_name_t    targa_tag("targa");
    GIL::rgba8_image        image;
    adobe::dictionary_t     params;

    assert (boost::filesystem::exists(test_src));

    boost::filesystem::filebuf filebuf;

    filebuf.open(test_src, std::ios_base::in | std::ios_base::binary);

    factory.register_format(format_type(targa_tag, GIL::targa_t<view_type>()));

    assert (factory.is_registered(targa_tag));

    assert (factory.read(image, filebuf, params) == targa_tag);

#if 0 // no file format writing at this time
    GIL::any_view view(GIL::view(image));

    factory.write(view, filebuf);
#endif

    factory.unregister_format(targa_tag);

    assert (!factory.is_registered(targa_tag));

    return 0;
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return 1;
}
catch (...)
{
    std::cerr << "Exception: Unknown" << std::endl;

    return 1;
}
