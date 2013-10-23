/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_METRICS_MAC_HPP
#define ADOBE_METRICS_MAC_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary_fwd.hpp>
#include <adobe/name_fwd.hpp>
#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

dictionary_t widget_metrics( const std::string& xstr);
dictionary_t widget_metrics( const std::string& xstr, const dictionary_t& context);

/**************************************************************************************************/

}

/**************************************************************************************************/

#endif

/**************************************************************************************************/
