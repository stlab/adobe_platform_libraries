/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_POPUP_COMMON_FWD_HPP
#define ADOBE_POPUP_COMMON_FWD_HPP

/****************************************************************************************************/

#include <boost/function.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/widget_attributes.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

typedef boost::function<void (const any_regular_t&)>              popup_setter_type;
typedef boost::function<void (const any_regular_t&, modifiers_t)> popup_extended_setter_type;

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_POPUP_COMMON_HPP
#endif

/****************************************************************************************************/
