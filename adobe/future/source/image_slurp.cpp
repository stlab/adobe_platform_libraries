/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/image_slurp.hpp>

#include <adobe/future/resources.hpp>
#include <adobe/gil/extension/asl_io/io_factory.hpp>
#include <adobe/gil/extension/asl_io/targa.hpp>
#include <adobe/name.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include <stdexcept>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

boost::gil::image_factory_t<boost::gil::rgba8_view_t>& gil_image_factory()
{
    typedef boost::gil::rgba8_view_t               view_type;
    typedef boost::gil::image_factory_t<view_type> factory_type;
    typedef factory_type::image_format_type format_type;

    static factory_type factory_s;
    static bool         inited(false);

    if (!inited)
    {
        inited = true;

        factory_s.register_format(format_type(adobe::static_name_t("targa"), boost::gil::targa_t<view_type>()));
    }

    return factory_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

void image_slurp(const boost::filesystem::path& path, boost::gil::rgba8_image_t& image)
{
    boost::filesystem::path actual_path(find_resource(path));

    boost::filesystem::filebuf filebuf;

    filebuf.open(actual_path, std::ios_base::in | std::ios_base::binary);

    dictionary_t params;

    gil_image_factory().read(image, filebuf, params);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
