/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_TABLE_HPP
#define ADOBE_SENTENCES_TABLE_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <vector>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <adobe/algorithm/partition_point.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/upper_bound.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename Key,                            // models Regular
          typename T,                              // models Regular
          typename Transform = mem_data_t<T, Key>, // models UnaryFunction Key(T)
          typename Compare   = std::less<Key> >    // models BinaryFunction bool(Key, Key)
struct table_index_traits
{
    typedef T         value_type;
    typedef Transform transform_type;
    typedef Key       key_type;
    typedef Compare   key_compare_type;
};

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

template <typename T, typename Transform, typename Compare>
struct transform_compare : std::binary_function<T, T, bool>
{
    typedef bool result_type;

    transform_compare(Transform& transform, const Compare& compare) :
        transform_m(transform),
        compare_m(compare)
    { }

    bool operator () (const T& x, const T& y) const
    { return compare_m(transform_m(x), transform_m(y)); }

private:
    Transform transform_m;
    Compare   compare_m;
};

/**************************************************************************************************/

template <typename Transform,
          typename Compare,
          typename Key = typename boost::remove_reference<typename Transform::result_type>::type>
struct lower_bound_predicate : std::unary_function<Key, bool>
{
    typedef typename Transform::argument_type argument_type;

    lower_bound_predicate(const Key& key, Transform& transform, const Compare& compare) :
        transform_m(transform),
        compare_m(compare),
        key_m(key)
    { }

    bool operator () (const argument_type& x) const
    { return !compare_m(transform_m(x), key_m); }

 private:
    Transform transform_m;
    Compare   compare_m;
    Key       key_m;
};

/**************************************************************************************************/

template <typename Transform,
          typename Compare,
          typename Key = typename boost::remove_reference<typename Transform::result_type>::type>
struct upper_bound_predicate : std::unary_function<Key, bool>
{
    typedef typename Transform::argument_type argument_type;

    upper_bound_predicate(const Key& key, Transform& transform, const Compare& compare) :
        transform_m(transform),
        compare_m(compare),
        key_m(key)
    { }

    bool operator () (const argument_type& x) const
    { return compare_m(key_m, transform_m(x)); }

 private:
    Transform transform_m;
    Compare   compare_m;
    Key       key_m;
};

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

template <typename T, typename Transform, typename Compare>
inline implementation::transform_compare<T, Transform, Compare>
make_transform_compare(Transform& t, const Compare& c)
{ return implementation::transform_compare<T, Transform, Compare>(t, c); }

template <typename K, typename T, typename C>
inline implementation::lower_bound_predicate<T, C>
make_lower_bound_predicate(const K& k, T& t, const C& c)
{ return implementation::lower_bound_predicate<T, C>(k, t, c); }

template <typename K, typename T, typename C>
inline implementation::upper_bound_predicate<T, C>
make_upper_bound_predicate(const K& k, T& t, const C& c)
{ return implementation::upper_bound_predicate<T, C>(k, t, c); }

/**************************************************************************************************/

template <typename T,
          typename IndexTraits1,
          typename IndexTraits2,
          typename IndexTraits3>
class table
{
public:
    typedef typename IndexTraits1::key_type         key1_type;
    typedef typename IndexTraits1::transform_type   transform1_type;
    typedef typename IndexTraits1::key_compare_type key_compare1_type;

    typedef typename IndexTraits2::key_type         key2_type;
    typedef typename IndexTraits2::transform_type   transform2_type;
    typedef typename IndexTraits2::key_compare_type key_compare2_type;

    typedef typename IndexTraits3::key_type         key3_type;
    typedef typename IndexTraits3::transform_type   transform3_type;
    typedef typename IndexTraits3::key_compare_type key_compare3_type;

    typedef std::vector<T>                   table_t;
    typedef typename table_t::iterator       iterator;
    typedef typename table_t::const_iterator const_iterator;
    typedef typename table_t::size_type      size_type;

    BOOST_STATIC_ASSERT((boost::is_same<typename IndexTraits1::value_type, T>::value));
    BOOST_STATIC_ASSERT((boost::is_same<typename IndexTraits2::value_type, T>::value));
    BOOST_STATIC_ASSERT((boost::is_same<typename IndexTraits3::value_type, T>::value));

    template <typename TransformPrimitive1, typename TransformPrimitive2, typename TransformPrimitive3>
    table(TransformPrimitive1      transform1,
          TransformPrimitive2      transform2,
          TransformPrimitive3      transform3,
          const key_compare1_type& compare1 = key_compare1_type(),
          const key_compare2_type& compare2 = key_compare2_type(),
          const key_compare3_type& compare3 = key_compare3_type()) :
        sorted_to_m(0),
        transform1_m(transform1),
        transform2_m(transform2),
        transform3_m(transform3),
        key_compare1_m(compare1),
        key_compare2_m(compare2),
        key_compare3_m(compare3)
    { }

    iterator insert(const T& value);
    iterator find(const T& value);
    void     erase(iterator position);
    void     erase(iterator first, iterator last);

    void      sort(size_type index = 0, bool force = false);
    size_type sorted_to() const { return sorted_to_m; }

    size_type count1(const key1_type& key) { return count(key, transform1_m, key_compare1_m, 1); }
    size_type count2(const key2_type& key) { return count(key, transform2_m, key_compare2_m, 2); }
    size_type count3(const key3_type& key) { return count(key, transform3_m, key_compare3_m, 3); }

    iterator lower_bound1(const key1_type& key) { return lower_bound(key, transform1_m, key_compare1_m, 1); }
    iterator lower_bound2(const key2_type& key) { return lower_bound(key, transform2_m, key_compare2_m, 2); }
    iterator lower_bound3(const key3_type& key) { return lower_bound(key, transform3_m, key_compare3_m, 3); }

    iterator upper_bound1(const key1_type& key) { return upper_bound(key, transform1_m, key_compare1_m, 1); }
    iterator upper_bound2(const key2_type& key) { return upper_bound(key, transform2_m, key_compare2_m, 2); }
    iterator upper_bound3(const key3_type& key) { return upper_bound(key, transform3_m, key_compare3_m, 3); }

    std::pair<iterator, iterator> equal_range1(const key1_type& key) { return equal_range(key, transform1_m, key_compare1_m, 1); }
    std::pair<iterator, iterator> equal_range2(const key2_type& key) { return equal_range(key, transform2_m, key_compare2_m, 2); }
    std::pair<iterator, iterator> equal_range3(const key3_type& key) { return equal_range(key, transform3_m, key_compare3_m, 3); }

    iterator       begin() { return table_m.begin(); }
    const_iterator begin() const { return table_m.begin(); }

    iterator       end() { return table_m.end(); }
    const_iterator end() const { return table_m.end(); }

private:
    template <typename Key, typename Transform, typename Compare>
    size_type count(const Key& key,
                    Transform& transform,
                    Compare&   compare,
                    size_type  index)
    {
        sort(index);

        return adobe::count_if(table_m, make_lower_bound_predicate(key, transform, compare));
    }

    template <typename Key, typename Transform, typename Compare>
    iterator lower_bound(const Key& key,
                         Transform& transform,
                         Compare&   compare,
                         size_type  index)
    {
        sort(index);

        return adobe::partition_point(table_m, make_lower_bound_predicate(key, transform, compare));
    }

    template <typename Key, typename Transform, typename Compare>
    iterator upper_bound(const Key& key,
                         Transform& transform,
                         Compare&   compare,
                         size_type  index)
    {
        sort(index);

        return adobe::partition_point(table_m, make_upper_bound_predicate(key, transform, compare));
    }

    template <typename Key, typename Transform, typename Compare>
    std::pair<iterator, iterator> equal_range(const Key& key,
                                              Transform& transform,
                                              Compare&   compare,
                                              size_type  index)
    {
        sort(index);

        return std::make_pair(lower_bound(key, transform, compare, index),
                              upper_bound(key, transform, compare, index));
    }

    table_t           table_m;
    size_type         sorted_to_m;
    transform1_type   transform1_m;
    transform2_type   transform2_m;
    transform3_type   transform3_m;
    key_compare1_type key_compare1_m;
    key_compare2_type key_compare2_m;
    key_compare3_type key_compare3_m;
};

/**************************************************************************************************/

template <typename T, typename Traits1, typename Traits2, typename Traits3>
typename table<T, Traits1, Traits2, Traits3>::iterator
table<T, Traits1, Traits2, Traits3>::insert(const T& value)
{
    sort();

    // Note: We do not use find here because it will always
    //       return end() (as this entry isn't in the table).
    iterator before(adobe::lower_bound(table_m, value));

    assert(before == end() || *before != value);

    return table_m.insert(before, value);
}

/**************************************************************************************************/

template <typename T, typename Traits1, typename Traits2, typename Traits3>
typename table<T, Traits1, Traits2, Traits3>::iterator
table<T, Traits1, Traits2, Traits3>::find(const T& value)
{
    sort();

    iterator pos(adobe::lower_bound(table_m, value));

    return pos != table_m.end() && *pos == value ? pos : table_m.end();
}

/**************************************************************************************************/

template <typename T, typename Traits1, typename Traits2, typename Traits3>
void table<T, Traits1, Traits2, Traits3>::sort(size_type to, bool force)
{
    if (to == sorted_to_m && force == false)
        return;

    if (to == 0)
        adobe::sort(table_m);
    else if (to == 1)
        adobe::sort(table_m, make_transform_compare<T>(transform1_m, key_compare1_m));
    else if (to == 2)
        adobe::sort(table_m, make_transform_compare<T>(transform2_m, key_compare2_m));
    else if (to == 3)
        adobe::sort(table_m, make_transform_compare<T>(transform3_m, key_compare3_m));
    else
        throw std::runtime_error("invalTraits sort index.");

    sorted_to_m = to;
}

/**************************************************************************************************/

template <typename T, typename Traits1, typename Traits2, typename Traits3>
void table<T, Traits1, Traits2, Traits3>::erase(iterator position)
{
    table_m.erase(position);
}

/**************************************************************************************************/

template <typename T, typename Traits1, typename Traits2, typename Traits3>
void table<T, Traits1, Traits2, Traits3>::erase(iterator first, iterator last)
{
    table_m.erase(first, last);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_TABLE_HPP
#endif

/**************************************************************************************************/
