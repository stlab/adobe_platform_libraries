/*
	Copyright 2008 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/operators.hpp>

#include <iostream>

#include <adobe/algorithm.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/sequence_mvc_muldex.hpp>

/******************************************************************************/

struct foo_t : boost::equality_comparable<foo_t>
{
    foo_t(int x = 0) :
        value_m(x)
    { }

    int value_m;
};

std::ostream& operator<<(std::ostream& s, const foo_t& x)
{ return s << x.value_m; }

bool operator==(const foo_t& x, const foo_t& y)
{ return x.value_m == y.value_m; }

/******************************************************************************/

template <typename T>
struct my_sequence_view
{
    typedef T                                value_type;
    typedef adobe::copy_on_write<value_type> cow_value_type;
    typedef typename adobe::sequence_key<T>  key_type;

    void refresh(key_type index, cow_value_type value)
    {
        std::cout << "  my_sequence_view::refresh " << index << " = " << value << ", yields: ";

        // *adobe::find(set_m, index) = value;

        print();
    }

    void extend(key_type before, key_type key, cow_value_type value)
    {
        std::cout << "  my_sequence_view::extend before: " << before << ", key: " << key << ", value: " << value << ", yields: ";

        set_m.insert(adobe::find(set_m, before), key);

        print();
    }

    void extend_set(key_type before, const adobe::vector<key_type>& extend_key_set)
    {
        std::cout << "  my_sequence_view::extend_set before: " << before << ", " << extend_key_set.size() << " element(s) yields: ";

        set_m.insert(adobe::find(set_m, before), extend_key_set.begin(), extend_key_set.end());

        print();
    }

    void erase(const adobe::vector<key_type>& key_set)
    {
        std::cout << "  my_sequence_view::erase " << key_set.size() << " element(s): ";

        std::cout << '{';
        for (typename std::vector<key_type>::const_iterator iter(key_set.begin()),
             last(key_set.end()); iter != last; ++iter)
        {
            std::cout << " " << *iter;
            set_m.erase(adobe::find(set_m, *iter));
        }
        std::cout << " }, yields: ";

        print();
    }

    void clear()
    {
        std::cout << "  my_sequence_view::clear, yields: ";

        print();
    }

    key_type key_for(std::size_t x) const
    { return set_m[x]; }

    cow_value_type operator[](key_type x) const
    { return adobe::sequence_model<T>::at(x); }

private:
    void print()
    {
        std::cout << '[';
        for (typename std::vector<key_type>::iterator iter(set_m.begin()),
             last(set_m.end()); iter != last; ++iter)
            std::cout << ' ' << adobe::sequence_model<T>::at(*iter);
        std::cout << " ]" << std::endl;
    }

    adobe::vector<key_type> set_m;
};

/******************************************************************************/

template <typename T>
struct my_sequence_controller
{
    typedef T                                            value_type;
    typedef adobe::sequence_key<T>                       key_type;
    typedef typename adobe::poly_sequence_model<T>::type sequence_type;

    /*
        This set of routines are called by the client to the controller
        to affect the model to which the controller is attached.
    */

    void push_back(const value_type& x)
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::push_back" << std::endl;

        sequence_m->push_back(x);
    }

    void set(key_type pos, const value_type& x)
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::set" << std::endl;

        sequence_m->set(pos, x);
    }

    void insert(key_type before, const value_type& x)
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::insert" << std::endl;

        sequence_m->insert(before, x);
    }

    void insert_set(key_type before, const adobe::vector<value_type>& x)
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::insert_set" << std::endl;

        sequence_m->insert_set(before, x);
    }

    void erase(const adobe::vector<key_type>& x)
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::erase" << std::endl;

        sequence_m->erase(x);
    }

    void clear()
    {
        if (!sequence_m)
            return;

        std::cout << "  my_sequence_controller::clear" << std::endl;

        sequence_m->clear();
    }

    /*
        These routines are called by the model of the controller to give
        the latter callback routines into the model to perform various
        commands.
    */

    void monitor_sequence(sequence_type& sequence)
    {
        std::cout << "  my_sequence_controller::monitor_sequence" << std::endl;

        sequence_m = &sequence;
    }

private:
    sequence_type* sequence_m;
};

/******************************************************************************/

template <typename T>
void flex(adobe::sequence_model<T>&  /*model*/,
          const my_sequence_view<T>& view,
          my_sequence_controller<T>& controller)
{
    controller.push_back(42);

    //BOOST_CHECK_EQUAL(model[0], view[0]);

    controller.push_back(42);

    //BOOST_CHECK_EQUAL(model[1], view[1]);

    controller.push_back(42);

    //BOOST_CHECK_EQUAL(model[2], view[2]);

    controller.push_back(42);

    //BOOST_CHECK_EQUAL(model[3], view[3]);

    controller.set(view.key_for(2), 84);

    //BOOST_CHECK_EQUAL(model[2], 84);
    //BOOST_CHECK_EQUAL(view[2], 84);

    controller.insert(view.key_for(2), 1024);

    //BOOST_CHECK_EQUAL(model.size(), 5ul);
    //BOOST_CHECK_EQUAL(view.size(), 5ul);
    //BOOST_CHECK_EQUAL(model[2], 1024);
    //BOOST_CHECK_EQUAL(view[2], 1024);

    adobe::vector<adobe::sequence_key<T> > key_set;

    key_set.push_back(view.key_for(1));
    key_set.push_back(view.key_for(3));

    controller.erase(key_set);

    //BOOST_CHECK_EQUAL(model.size(), 3ul);
    //BOOST_CHECK_EQUAL(view.size(), 3ul);

    adobe::vector<T> insert_set;

    insert_set.push_back(1);
    insert_set.push_back(2);
    insert_set.push_back(3);
    insert_set.push_back(4);
    insert_set.push_back(5);

    controller.insert_set(view.key_for(2), insert_set);
}

/******************************************************************************/

BOOST_AUTO_TEST_CASE(muldex_property_model)
{
    std::cout << "<muldex_property_model>" << std::endl;

    adobe::sheet_t               sequence_view_property_model;
    adobe::sheet_t               sequence_controller_property_model;
    adobe::sequence_model<foo_t> sequence_model;
    adobe::array_t               dictionary_initializer(1, adobe::any_regular_t(adobe::dictionary_t()));
    adobe::assemblage_t          assemblage;

    /*
        This cell will send messages to the list widget; these messages
        will likely be one of the refresh, extend, and erase messages.
        The primary controller setting values for this cell will be the
        sequence model itself. The primary view will be the view portion
        of the list widget.
    */
    sequence_view_property_model.add_interface(adobe::static_name_t("line"), true,
                                               adobe::line_position_t(), dictionary_initializer,
                                               adobe::line_position_t(), adobe::array_t());

    sequence_view_property_model.update();

    /*
        This cell will send messages to the sequence model; these
        messages will likely be one of the push_back, etc. The primary
        controller setting values for this cell will be the list
        widget(s) connected to the sequence model. The primary view will
        be the sequence model.
    */
    sequence_controller_property_model.add_interface(adobe::static_name_t("line"), true,
                                                     adobe::line_position_t(), dictionary_initializer,
                                                     adobe::line_position_t(), adobe::array_t());

    sequence_controller_property_model.update();

    my_sequence_view<foo_t>       my_sequence_view;
    my_sequence_controller<foo_t> my_sequence_controller;

    attach_sequence_view_muldex(sequence_model,
                                my_sequence_view,
                                sequence_view_property_model,
                                adobe::static_name_t("line"),
                                assemblage);

    attach_sequence_controller_muldex(sequence_model,
                                      my_sequence_controller,
                                      sequence_controller_property_model,
                                      adobe::static_name_t("line"),
                                      assemblage);

    sequence_view_property_model.update();
    sequence_controller_property_model.update();

    flex(sequence_model, my_sequence_view, my_sequence_controller);

    std::cout << "</muldex_property_model>" << std::endl;
}

/******************************************************************************/

BOOST_AUTO_TEST_CASE(muldex_basic_sheet)
{
    std::cout << "<muldex_basic_sheet>" << std::endl;

    adobe::sheet_t               basic_sheet;
    adobe::sequence_model<foo_t> sequence_model;
    adobe::any_regular_t         dictionary_initializer;
    adobe::assemblage_t          assemblage;

    dictionary_initializer = adobe::any_regular_t(adobe::dictionary_t());

    /*
        This cell will send messages to the list widget; these messages will likely
        be one of the refresh, extend, and erase messages. The primary controller
        setting values for this cell will be the sequence model itself. The primary
        view will be the view portion of the list widget.
    */
    basic_sheet.add_interface(adobe::static_name_t("line_in"), dictionary_initializer);
    basic_sheet.add_interface(adobe::static_name_t("line_out"), dictionary_initializer);

    my_sequence_view<foo_t>       my_sequence_view;
    my_sequence_controller<foo_t> my_sequence_controller;

    attach_sequence_view_muldex(sequence_model,
                                my_sequence_view,
                                basic_sheet,
                                adobe::static_name_t("line_out"),
                                assemblage);

    attach_sequence_controller_muldex(sequence_model,
                                      my_sequence_controller,
                                      basic_sheet,
                                      adobe::static_name_t("line_in"),
                                      assemblage);

    flex(sequence_model, my_sequence_view, my_sequence_controller);

    std::cout << "</muldex_basic_sheet>" << std::endl;
}

/******************************************************************************/
