/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/behavior.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

/**************************************************************************************************/

namespace {

/*
    REVISIT (sparent) : Fixing for C++11 (XCode 4.4). behavior_token_t was an iterator - but that
    causes a circular dependency on behavior_t to itself. Changed to a pointer but should probably
    be an index. Quick code here to move from iterator to pointer and the reverse.
*/

template <typename T, // T models contiguous container
          typename I> // I models iterator to container
typename T::pointer_type iterator_to_pointer(T& x, I i) {
    return x.empty() ? 0 : &x.front() + (i - x.begin());
}

template <typename T, // T models contiguous container
          typename I> // I models iterator to container
typename T::const_pointer_type iterator_to_pointer(const T& x, I i) {
    return x.empty() ? 0 : &x.front() + (i - x.begin());
}


template <typename T, // T models contiguous container
          typename P> // P models pointer into container
typename T::iterator pointer_to_iterator(T& x, P p) {
    return x.empty() ? x.end() : x.begin() + (p - &x.front());
}

template <typename T, // T models contiguous container
          typename P> // P models pointer into container
typename T::const_iterator pointer_to_iterator(const T& x, P p) {
    return x.empty() ? x.end() : x.begin() + (p - &x.front());
}

} // namespace

namespace adobe {

/**************************************************************************************************/

behavior_t::behavior_t(bool single_execution) :
    single_execution_m(single_execution),
    behavior_set_m(new behavior_set_t)
{ }

/**************************************************************************************************/

void behavior_t::operator () ()
{
    assert(order_set_m.size() == verb_set_m.size() + behavior_set_m->size());

    if (order_set_m.empty())
        return;

    verb_set_t::iterator     vfirst(verb_set_m.begin());
    behavior_set_t::iterator bfirst(behavior_set_m->begin());
    order_set_t::iterator    first(order_set_m.begin());
    order_set_t::iterator    last(order_set_m.end());

    for (; first != last; ++first)
    {
        if (*first == behavior_t::order_verb_k)
        {
            (*vfirst)();

            ++vfirst;
        }
        else if (*first == behavior_t::order_behavior_k)
        {
            (*bfirst)();

            ++bfirst;
        }
    }

    if (single_execution_m)
    {
        behavior_set_m->clear();
        verb_set_m.clear();
        order_set_m.clear();
    }
}

/**************************************************************************************************/

behavior_t::behavior_token_t behavior_t::insert_behavior(bool single_execution)
{
    order_set_m.push_back(behavior_t::order_behavior_k);

    behavior_set_m->push_back(behavior_t(single_execution));

    return &behavior_set_m->back();
}

/**************************************************************************************************/

behavior_t::verb_token_t behavior_t::insert(const verb_t& verb)
{
    order_set_m.push_back(behavior_t::order_verb_k);

    verb_set_m.push_back(verb);

    return --(verb_set_m.end());
}

/**************************************************************************************************/

void behavior_t::reset(verb_token_t token, const verb_t& verb)
{
    *token = verb;
}

/**************************************************************************************************/

void behavior_t::disconnect(behavior_token_t behavior)
{
    assert(behavior_set_m->empty() == false);
    
    behavior_set_t::iterator p;
    std::size_t n = 0;
    
    for (auto f = std::begin(*behavior_set_m), l = std::end(*behavior_set_m); f != l; ++f) {
        if (&*f == behavior) { p = f; break; }
        ++n;
    }

    behavior_set_m->erase(p);
    erase_from_order(order_behavior_k, n);
}

/**************************************************************************************************/

void behavior_t::disconnect(verb_token_t verb)
{
    assert(verb_set_m.empty() == false);

    std::size_t index(static_cast<std::size_t>(std::distance(verb_set_m.begin(), verb)));

    verb_set_m.erase(verb);

    erase_from_order(order_verb_k, index);
}

/**************************************************************************************************/

void behavior_t::erase_from_order(order_t type, std::size_t index)
{
    order_set_t::iterator first(order_set_m.begin());
    order_set_t::iterator last(order_set_m.end());
    std::size_t           i(0);

    while (first != last)
    {
        if (*first == type)
        {
            if (i == index)
            {
                order_set_m.erase(first);

                break;
            }
            else
            {
                ++i;
            }
        }

        ++first;
    }
}

/**************************************************************************************************/

behavior_t& general_deferred_proc_queue()
{
    static behavior_t gdpq_s(true);

    return gdpq_s;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
