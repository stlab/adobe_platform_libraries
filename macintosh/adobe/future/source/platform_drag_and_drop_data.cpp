/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <adobe/future/platform_drag_and_drop_data.hpp>

#include <adobe/future/macintosh_filesystem.hpp>
#include <adobe/macintosh_memory.hpp>
#include <adobe/macintosh_string.hpp>
#include <adobe/name.hpp>

#include <vector>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

aggregate_name_t key_drag_ref  = { "drag_ref" };
aggregate_name_t key_drag_item = { "drag_item" };
aggregate_name_t key_flavor    = { "flavor" };

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

std::string file_url_to_path(const std::string& src)
{
    auto_cfstring_t             cfstring(explicit_cast<auto_cfstring_t>(src));
    auto_resource< ::CFURLRef > url;

    url.reset(::CFURLCreateWithString(NULL, cfstring.get(), NULL));

    ::FSRef fs_ref = { { 0 } };

    if (::CFURLGetFSRef(url.get(), &fs_ref) == false)
        throw std::runtime_error("Could not convert URL to a path.");

    boost::filesystem::path bfs_path(adobe::fsref_to_path(fs_ref));

    return bfs_path.string();
};

/**************************************************************************************************/

any_regular_t flavor_extract_character_sequence(const dictionary_t& drag_parameters)
{
    ::Size          data_size(0);
    ::DragRef       the_drag(0);
    ::DragItemRef   the_item(0);
    boost::uint32_t the_flavor(0);

    get_value(drag_parameters, key_drag_ref).cast(the_drag);
    get_value(drag_parameters, key_drag_item).cast(the_item);
    get_value(drag_parameters, key_flavor).cast(the_flavor);

    ::GetFlavorDataSize(the_drag, the_item, the_flavor, &data_size);

    std::vector<char> raw_data;

    raw_data.resize(data_size + 1);

    ::GetFlavorData(the_drag, the_item, the_flavor, &raw_data[0], &data_size, 0);

    raw_data[data_size] = 0;

    return any_regular_t(std::string(&raw_data[0]));
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
