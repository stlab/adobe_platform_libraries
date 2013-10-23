/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/algorithm.hpp>
#include <adobe/selection.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/utility/enable_if.hpp>

#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::vector<int>         set_type;
typedef set_type::iterator       iterator;
typedef adobe::selection_t       selection_type;
typedef selection_type::iterator selection_iterator;

/****************************************************************************************************/

template <typename ForwardRange>  
void print_selection(const ForwardRange& range, const adobe::selection_t& selection)
{
    typedef typename ForwardRange::const_iterator       set_const_iterator;
    typedef typename adobe::selection_t::const_iterator selection_const_iterator;

    set_const_iterator       iter(boost::begin(range));
    set_const_iterator       last(boost::end(range));
    selection_const_iterator s_iter(boost::begin(selection));
    selection_const_iterator s_last(boost::end(selection));
    bool                     inside(selection.start_selected());
    std::stringstream        selection_output;

    while (iter != last)
    {
        if (s_iter != s_last && iter == boost::next(boost::begin(range), *s_iter))
        {
            inside = !inside;

            ++s_iter;
        }

        std::stringstream foo;

        foo << *iter << ' ';

        selection_output.fill(inside ? '*' : ' ');
        selection_output.width(static_cast<std::streamsize>(foo.str().size()));
        selection_output << (inside ? '*' : ' ');

        std::cout << foo.str().c_str();

        ++iter;
    }

    std::cout << std::endl << selection_output.str().c_str() << std::endl;

    std::cout << adobe::size(selection) << " boundaries, "
              << adobe::size(selection, range) << " items: [ ";

    selection_copy(selection, range,
                   std::ostream_iterator<typename ForwardRange::value_type>(std::cout, " "));

    std::cout << ']' << std::endl;
}

/****************************************************************************************************/

inline void is_odd(std::size_t x)
{
    std::cout << x << " is in the selection and is " << (x % 2 ? "odd" : "even") << std::endl;
}

/****************************************************************************************************/

void test1()
{
    set_type set(20);

    adobe::iota(set.begin(), set.end(), 0);

    selection_type selection1;

    selection1.push_back(2);
    selection1.push_back(5);
    selection1.push_back(7);
    selection1.push_back(8);
    selection1.push_back(10);
    selection1.push_back(13);
    selection1.push_back(15);
    selection1.push_back(18);

    selection_type selection2;

    selection2.push_back(1);
    selection2.push_back(3);
    selection2.push_back(5);
    selection2.push_back(6);
    selection2.push_back(8);
    selection2.push_back(9);
    selection2.push_back(11);
    selection2.push_back(12);
    selection2.push_back(14);
    selection2.push_back(15);
    selection2.push_back(17);

    selection_type selection3;

    selection3.push_back(2);
    selection3.push_back(4);
    selection3.push_back(7);
    selection3.push_back(8);
    selection3.push_back(11);
    selection3.push_back(12);
    selection3.push_back(16);
    selection3.push_back(18);

    std::cout << "selection 1:" << std::endl;
    print_selection(set, selection1);

    std::cout << "selection 2:" << std::endl;
    print_selection(set, selection2);

    std::cout << "selection 3:" << std::endl;
    print_selection(set, selection3);

    invert(selection3);

    std::cout << "selection 3 (inverted):" << std::endl;
    print_selection(set, selection3);

    invert(selection3);

    {
    std::cout << "selection_intersection:" << std::endl;

    selection_type result = adobe::selection_intersection(selection1, selection2);

    print_selection(set, result);
    }

    {
    std::cout << "selection_union:" << std::endl;

    selection_type result = adobe::selection_union(selection1, selection2);

    print_selection(set, result);
    }

    {
    std::cout << "selection_difference:" << std::endl;

    selection_type result = adobe::selection_difference(selection1, selection2);

    print_selection(set, result);
    }

    {
    std::cout << "selection_symmetric_difference:" << std::endl;

    selection_type result = adobe::selection_symmetric_difference(selection1, selection2);

    print_selection(set, result);
    }

    std::cout << "src1 includes src3? " << std::boolalpha << adobe::selection_includes(selection1, selection3) << std::endl;

    std::cout << "src3 includes src1? " << std::boolalpha << adobe::selection_includes(selection3, selection1) << std::endl;

    for (std::size_t i(0); i < 20; ++i)
        std::cout << "index " << i << " within selection 1: "
                  << std::boolalpha << adobe::is_selected(selection1, i)
                  << std::endl;

    std::cout << std::endl;

    selection_foreach(selection1, set, &is_odd);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main()
{
    std::cout << "test1:" << std::endl;
    test1();

    return 0;
}

/****************************************************************************************************/
