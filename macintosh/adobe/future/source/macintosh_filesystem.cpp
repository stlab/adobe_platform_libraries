/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/future/macintosh_filesystem.hpp>

#include <adobe/future/macintosh_error.hpp>

#include <vector>

/**************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/**
    Convert a MacOS FSRef into a Boost path. This allows us to use the
    boost::filesystem library to perform operations on the path, rather
    than using the FSRef API.

    \param  location    the FSRef to make into a bfs::path
    \return a bfs path of the given location.
*/
boost::filesystem::path fsref_to_path(const ::FSRef& location)
{
    std::size_t       max_size (1024);
    std::vector<char> path_buffer(max_size);
    OSStatus          error(noErr);

    while (true)
    {
        error = ::FSRefMakePath(&location, reinterpret_cast<UInt8*>(&path_buffer[0]), max_size);

        if (error != pathTooLongErr)
            break;

        max_size *= 2;
        path_buffer.resize(max_size);
    }

    ADOBE_REQUIRE_STATUS(error);

    return boost::filesystem::path(&path_buffer[0]);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
