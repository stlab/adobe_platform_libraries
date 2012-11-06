#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>

#include <adobe/forest_bitpath.hpp>

/******************************************************************************/

template <typename T>
struct forest_map
{
    typedef std::map<adobe::bitpath_t, typename adobe::forest<T>::iterator> type;
};

/******************************************************************************/

template <typename R> // R is a depth adaptor range
void output(const R& f)
{
    typedef typename boost::range_iterator<R>::type iterator;

    for (iterator first(boost::begin(f)), last(boost::end(f)); first != last; ++first)
        std::cout << adobe::indents(first.depth())
                  << (first.edge() == adobe::forest_leading_edge ? "<" : "</")
                  << *first << ">" << std::endl;
}

/******************************************************************************/

template <typename I>
inline bool has_next_sibling(I i)
{
    return (++adobe::trailing_of(i)).edge() == adobe::forest_leading_edge;
}

/******************************************************************************/

template <typename T>
inline typename adobe::forest<T>::iterator find_index_parent(typename forest_map<T>::type& map,
                                                             const adobe::bitpath_t&       path)
{
    adobe::bitpath_t parent(parent_of(path));

    if (parent.empty())
        return typename adobe::forest<T>::iterator();

    typename forest_map<T>::type::const_iterator found(map.find(parent));

    if (found == map.end())
        throw std::runtime_error("Index not found.");

    return found->second;
}

/******************************************************************************/

template <typename Forest>
adobe::bitpath_t iterator_to_index(const Forest&                   forest,
                                   typename Forest::const_iterator iter)
{
    return adobe::bitpath_t(iter, typename Forest::const_iterator(const_cast<Forest&>(forest).root()));
}

/******************************************************************************/

template <typename Forest>
typename Forest::const_iterator index_to_iterator(const Forest&           forest,
                                                  const adobe::bitpath_t& path)
{
    return traverse(path, forest);
}

/******************************************************************************/

template <typename T>
void build_index_map(typename forest_map<T>::type&       map,
                     adobe::bitpath_t                    key,
                     typename adobe::forest<T>::iterator value)
{
#ifndef NDEBUG
    const T& debug_val(*value);
    #pragma unused(debug_val)
#endif

    map[key] = value;

    adobe::bitpath_t child_key(adobe::first_child_of(key));
    adobe::bitpath_t sibling_key(adobe::next_sibling_of(key));

    // add the first child if one exists (the left branch)
    if (adobe::has_children(value))
        build_index_map<T>(map, child_key, ++adobe::leading_of(value));

    // add the next sibling if one exists (the right branch)
    if (has_next_sibling(value))
        build_index_map<T>(map, sibling_key, ++adobe::trailing_of(value));
}

/******************************************************************************/

int main()
try
{
    adobe::forest<char> forest;

    adobe::forest<char>::iterator a(forest.insert(forest.begin(), 'a'));

    a = adobe::trailing_of(a);

    adobe::forest<char>::iterator b(forest.insert(a, 'b'));
    adobe::forest<char>::iterator c(forest.insert(a, 'c'));

    b = adobe::trailing_of(b);

    forest.insert(b, 'd');
    forest.insert(b, 'e');
    forest.insert(b, 'f');

    forest.insert(adobe::trailing_of(forest.insert(adobe::trailing_of(c), 'g')), 'h');

    output(depth_range(forest));

    forest_map<char>::type index_map;

    {
        adobe::forest<char>::const_preorder_iterator iter(forest.begin());
        adobe::forest<char>::const_preorder_iterator last(forest.end());

        for (; iter != last; ++iter)
        {
            std::cout << *iter << " has_next_sibling : " << std::boolalpha
                      << has_next_sibling(iter.base()) << std::endl;
        }
    }

    build_index_map<char>(index_map, adobe::bitpath_t(), forest.begin());

    {
        forest_map<char>::type::const_iterator iter(index_map.begin());
        forest_map<char>::type::const_iterator last(index_map.end());

        for (; iter != last; ++iter)
        {
            std::cout << *iter->second << " has index : "
                      << iter->first << std::endl;
        }
    }

    {
        forest_map<char>::type::const_iterator iter(index_map.begin());
        forest_map<char>::type::const_iterator last(index_map.end());

        for (; iter != last; ++iter)
        {
            adobe::forest<char>::iterator parent(find_index_parent<char>(index_map, iter->first));

            if (parent == adobe::forest<char>::iterator())
            {
                std::cout << *iter->second << " has no parent." << std::endl;
            }
            else
            {
                std::cout << *iter->second << " has parent : "
                          << *parent << std::endl;
            }
        }
    }

    {
        adobe::forest<char>::preorder_iterator iter(forest.begin());
        adobe::forest<char>::preorder_iterator last(forest.end());

        for (; iter != last; ++iter)
        {
            std::cout << *iter << " has index : "
                      << iterator_to_index(forest, iter.base()) << std::endl;
        }
    }

    {
        forest_map<char>::type::const_iterator iter(index_map.begin());
        forest_map<char>::type::const_iterator last(index_map.end());

        for (; iter != last; ++iter)
        {
            std::cout << "index " << iter->first << " has value "
                      << *index_to_iterator(forest, iter->first) << std::endl;
        }
    }

    {
        forest_map<char>::type::const_iterator iter(index_map.begin());
        forest_map<char>::type::const_iterator last(index_map.end());

        for (; iter != last; ++iter)
        {
            adobe::bitpath_t             path(iter->first);
            adobe::vector<unsigned char> exported(path.portable());
            adobe::bitpath_t             imported(exported);

            std::cout << "path: " << path
                      << ", round-trip: " << imported << std::endl;
        }
    }

    {
        adobe::bitpath_t small_path;

        small_path.push(1);
        small_path.push(0);
        small_path.push(1);
        small_path.push(1);
        small_path.push(0);

        adobe::bitpath_t med_path(small_path);

        med_path.push(0);
        med_path.push(1);
        med_path.push(1);
        med_path.push(0);
        med_path.push(1);

        adobe::bitpath_t long_path(med_path);

        long_path.push(1);
        long_path.push(0);
        long_path.push(0);
        long_path.push(1);
        long_path.push(1);

        std::cout << "small_path: " << small_path << std::endl;
        std::cout << "  med_path: " << med_path << std::endl;
        std::cout << " long_path: " << long_path << std::endl;

        std::cout << "pass_through(invalid, invalid): " << std::boolalpha << passes_through(adobe::nbitpath(), adobe::nbitpath()) << std::endl;
        std::cout << "pass_through(invalid, small): " << std::boolalpha << passes_through(adobe::nbitpath(), small_path) << std::endl;
        std::cout << "pass_through(invalid, med): " << std::boolalpha << passes_through(adobe::nbitpath(), med_path) << std::endl;
        std::cout << "pass_through(invalid, long): " << std::boolalpha << passes_through(adobe::nbitpath(), long_path) << std::endl;

        std::cout << "pass_through(small, invalid): " << std::boolalpha << passes_through(small_path, adobe::nbitpath()) << std::endl;
        std::cout << "pass_through(small, small): " << std::boolalpha << passes_through(small_path, small_path) << std::endl;
        std::cout << "pass_through(small, med): " << std::boolalpha << passes_through(small_path, med_path) << std::endl;
        std::cout << "pass_through(small, long): " << std::boolalpha << passes_through(small_path, long_path) << std::endl;

        std::cout << "pass_through(med, invalid): " << std::boolalpha << passes_through(med_path, adobe::nbitpath()) << std::endl;
        std::cout << "pass_through(med, small): " << std::boolalpha << passes_through(med_path, small_path) << std::endl;
        std::cout << "pass_through(med, med): " << std::boolalpha << passes_through(med_path, med_path) << std::endl;
        std::cout << "pass_through(med, long): " << std::boolalpha << passes_through(med_path, long_path) << std::endl;

        std::cout << "pass_through(long, invalid): " << std::boolalpha << passes_through(long_path, adobe::nbitpath()) << std::endl;
        std::cout << "pass_through(long, small): " << std::boolalpha << passes_through(long_path, small_path) << std::endl;
        std::cout << "pass_through(long, med): " << std::boolalpha << passes_through(long_path, med_path) << std::endl;
        std::cout << "pass_through(long, long): " << std::boolalpha << passes_through(long_path, long_path) << std::endl;
    }

    return 0;
}
catch(const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;
    return 1;
}
catch(...)
{
    std::cerr << "Exception: unknown" << std::endl;
    return 1;
}

/******************************************************************************/
