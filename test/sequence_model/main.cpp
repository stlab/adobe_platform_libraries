/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <adobe/algorithm/find.hpp>
#include <adobe/poly_sequence_model.hpp>
#include <adobe/sequence_model.hpp>
#include <adobe/sequence_hooks.hpp>

/******************************************************************************/

namespace {

/******************************************************************************/

struct my_struct
{ };

/******************************************************************************/

template <typename T>
struct test_controller
{
    typedef T                                            value_type;
    typedef typename adobe::poly_sequence_model<T>::type poly_sequence_model_type;

    void monitor_sequence(poly_sequence_model_type& sequence)
    { sequence_m = &sequence; }

    poly_sequence_model_type* sequence_m;
};

/******************************************************************************/

template <typename T>
struct test_view
{
    typedef T                                          value_type;
    typedef adobe::copy_on_write<value_type>           cow_value_type;
    typedef typename adobe::sequence_key<T>            key_type;

    void refresh(key_type index, cow_value_type value)
    {
        std::cout << "refresh " << index << " = " << value << std::endl;

        print();
    }

    void extend(key_type before, key_type key, cow_value_type value)
    {
        std::cout << "extend before: " << before << ", key: " << key << ", value: " << value << std::endl;

        set_m.insert(adobe::find(set_m, before), key);

        print();
    }

    void extend_set(key_type before, const adobe::vector<key_type>& key_set)
    {
        std::cout << "extend before: " << before << ", " << key_set.size() << " key(s)" << std::endl;

        set_m.insert(adobe::find(set_m, before), key_set.begin(), key_set.end());

        print();
    }

    void erase(const adobe::vector<key_type>& key_set)
    {
        std::cout << "erase " << key_set.size() << " element(s) ";

        std::cout << '{';
        for (typename std::vector<key_type>::const_iterator iter(key_set.begin()),
             last(key_set.end()); iter != last; ++iter)
        {
            std::cout << " " << *iter;
            set_m.erase(adobe::find(set_m, *iter));
        }
        std::cout << " }" << std::endl;

        print();
    }

    void clear()
    {
        std::cout << "clear" << std::endl;

        set_m.clear();

        print();
    }

    key_type key_for(std::size_t index)
    { return set_m[index]; }

private:
    void print()
    {
        std::cout << " [";
        for (typename std::vector<key_type>::iterator iter(set_m.begin()),
             last(set_m.end()); iter != last; ++iter)
            std::cout << " " << adobe::sequence_model<T>::at(*iter);
        std::cout << " ]" << std::endl;
    }

    std::vector<key_type> set_m;
};

/******************************************************************************/

} // namespace

/******************************************************************************/

int main(int /*argc*/, char** /*argv*/)
try
{
    std::cout << "adobe::sequence_model<int>::interface_requires_std_rtti: "
              << std::boolalpha << adobe::sequence_model<int>::interface_requires_std_rtti() << std::endl;
    std::cout << "adobe::sequence_model<my_struct>::interface_requires_std_rtti: "
              << std::boolalpha << adobe::sequence_model<my_struct>::interface_requires_std_rtti() << std::endl;

    std::cout << "type_info<adobe::sequence_model<int>::key_type>(): "
              << adobe::type_info<adobe::sequence_model<int>::key_type>() << std::endl;
    std::cout << "type_info<adobe::sequence_model<int>::key_type>(): "
              << adobe::type_info<adobe::sequence_model<my_struct>::key_type>() << std::endl;

    std::cout << std::endl;

    typedef adobe::sequence_key<int>   key_type;
    typedef adobe::sequence_model<int> model_type;
    typedef test_controller<int>       controller_type;
    typedef test_view<int>             view_type;
    typedef model_type::key_type       key_type;

    model_type          model;
    controller_type     controller;
    view_type           view;
    adobe::assemblage_t assemblage;

    attach_sequence_controller_to_sequence_model(assemblage, model, controller);
    attach_sequence_view_to_sequence_model(assemblage, model, view);

    assert(controller.sequence_m);

    controller.sequence_m->push_back(42);
    controller.sequence_m->push_back(1024);
    controller.sequence_m->push_back(2048);
    controller.sequence_m->push_back(4096);
    controller.sequence_m->push_back(8192);

    controller.sequence_m->set(view.key_for(1), 777);

    adobe::vector<key_type> erase_set;

    erase_set.push_back(view.key_for(0));
    erase_set.push_back(view.key_for(2));

    controller.sequence_m->erase(erase_set);

    adobe::vector<int> insert_set;

    insert_set.push_back(1);
    insert_set.push_back(2);
    insert_set.push_back(3);
    insert_set.push_back(4);
    insert_set.push_back(5);

    controller.sequence_m->insert_set(view.key_for(1), insert_set);

    controller.sequence_m->clear();
}
catch (const std::exception& error)
{
    std::cerr << "Top-level exception: " << error.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Top-level exception: unknown" << std::endl;
    return 1;
}

/******************************************************************************/
