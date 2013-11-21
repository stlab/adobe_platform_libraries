/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#include <adobe/dictionary_set.hpp>

#include <adobe/algorithm/set.hpp>
#include <adobe/table_index.hpp>

/*************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

dictionary_t dictionary_union(const dictionary_t& src1,
                              const dictionary_t& src2)
{
    if (src1.empty())
        return src2;
    else if (src2.empty())
        return src1;

    dictionary_t dst;

    typedef table_index<const name_t, const dictionary_t::value_type> index_type;

    index_type index1(src1.begin(), src1.end(), &dictionary_t::value_type::first);
    index_type index2(src2.begin(), src2.end(), &dictionary_t::value_type::first);

    index1.sort();
    index2.sort();

    set_union(index1, index2, std::inserter(dst, dst.end()),
        compare_members(&dictionary_t::value_type::first));

    return dst;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
