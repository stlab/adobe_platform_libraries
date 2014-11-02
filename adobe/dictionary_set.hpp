/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_DICTIONARY_SET_HPP
#define ADOBE_DICTIONARY_SET_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
    Performs a set union between two dictionaries.
    
    dictionary_t is a hash-based associative container, so does not order
    elements lexicographically. This routine creates temporary table indices to
    sort the dictionary entries lexicographically by key, then performs the
    set_union.

    See Also:
        - http://www.sgi.com/tech/stl/set_union.html
*/
dictionary_t dictionary_union(const dictionary_t& src1,
                              const dictionary_t& src2);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
// ADOBE_DICTIONARY_SET_HPP
#endif

/**************************************************************************************************/
