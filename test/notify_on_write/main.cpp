/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/notify_on_write.hpp>

#include <adobe/algorithm.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <vector>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <typename T, typename MapType = std::size_t>
struct notify_proc
{
    typedef T                                                    value_type;
    typedef MapType                                              map_type;
    typedef notify_proc<value_type, map_type>                    notify_proc_type;
    typedef adobe::notify_on_write<value_type, notify_proc_type> notify_type;
    typedef boost::function<map_type (const notify_type&)>       mapper_t;

    explicit notify_proc(const mapper_t& mapper = mapper_t()) :
        mapper_m(mapper)
    { }

    void ctor(const notify_type& now) const
    {
        const value_type& value(*now);

        std::cout << typeid(value_type).name() << " ctor ";
        if (mapper_m)
            std::cout << '[' << mapper_m(now) << ']';
        else
            std::cout << reinterpret_cast<std::size_t>(&value);
        std::cout << " with: " << value << std::endl;
    }

    void dtor(const notify_type& now) const
    {
        const value_type& value(*now);

        std::cout << typeid(value_type).name() << " dtor ";
        if (mapper_m)
            std::cout << '[' << mapper_m(now) << ']';
        else
            std::cout << reinterpret_cast<std::size_t>(&value);
        std::cout << std::endl;
    }

    void modify(const notify_type& now) const
    {
        const value_type& value(*now);

        std::cout << typeid(value_type).name() << " modify ";
        if (mapper_m)
            std::cout << '[' << mapper_m(now) << ']';
        else
            std::cout << reinterpret_cast<std::size_t>(&value);
        std::cout << " to: " << value << std::endl;
    }

    mapper_t mapper_m;
};

/**************************************************************************************************/

template <typename C>
std::size_t index_within(const C& c, const typename C::value_type& x)
{
    typedef typename C::value_type value_type;

    if (c.empty())
        return reinterpret_cast<std::size_t>(&x);

    const value_type* iter(&(*boost::begin(c)));
    const value_type* last(boost::next(iter, c.size()));
    std::size_t       count(0);

    while (iter != last)
    {
        if (iter == &x)
            break;

        ++iter;
        ++count;
    }

    return iter == last ? reinterpret_cast<std::size_t>(&x) : count;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

template <typename C>
inline adobe::notify_on_write<typename C::value_type::value_type,
                              notify_proc<typename C::value_type::value_type> >
make_contained_now(const typename C::value_type::value_type& value, const C& container)
{
    typedef typename C::value_type::value_type value_type;

    notify_proc<value_type> proc(boost::bind(&index_within<C>, boost::cref(container), _1));

    return adobe::notify_on_write<value_type, notify_proc<value_type> >(value, proc);
}

/**************************************************************************************************/

int main(int argc, char** argv)
{
    typedef adobe::notify_on_write<long, notify_proc<long> > notify_int_t;
    typedef std::vector<notify_int_t>                        notify_set_t;

    {
    notify_int_t foo(5);   // notify ctor 5
    foo = 42;              // notify mod 42
    notify_int_t bar(foo); // notify ctor 42
    bar = 7;               // notify mod 7
    // notify dtor (bar)
    // notify dtor (foo)
    }

    std::cout << "notify_set_t ctor" << std::endl;
    notify_set_t set;
    std::cout << "set.push_back(notify_int_t(12));" << std::endl;
    set.push_back(make_contained_now(12, set));
    std::cout << "set.push_back(notify_int_t(34));" << std::endl;
    set.push_back(make_contained_now(34, set));
    std::cout << "set.push_back(notify_int_t(56));" << std::endl;
    set.push_back(make_contained_now(56, set));
    std::cout << "set.push_back(notify_int_t(78));" << std::endl;
    set.push_back(make_contained_now(78, set));
    std::cout << "set[2] = 90;" << std::endl;
    set[2] = 90;
    std::cout << "set.insert(begin + 2, 42);" << std::endl;
    set.insert(boost::next(set.begin(), 2), make_contained_now(42, set));
    std::cout << "<end>" << std::endl;
}
