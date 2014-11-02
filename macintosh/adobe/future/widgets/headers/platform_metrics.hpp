/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_METRICS_MAC_HPP
#define ADOBE_METRICS_MAC_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary_fwd.hpp>
#include <adobe/name.hpp>
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
