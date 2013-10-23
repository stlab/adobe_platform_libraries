/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_CARBON_FILESYSTEM_HPP
#define ADOBE_CARBON_FILESYSTEM_HPP

/**************************************************************************************************/

#include <adobe/macintosh_carbon_safe.hpp>

#include <boost/filesystem/path.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

boost::filesystem::path fsref_to_path(const ::FSRef& location);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
