/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_KEYBOARD_HPP
#define ADOBE_KEYBOARD_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/poly_key_handler.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/forest.hpp>
#include <adobe/widget_attributes.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct keyboard_t
{
    typedef forest<poly_key_handler_t> keyboard_forest_t;
    typedef keyboard_forest_t::iterator      iterator;

    static keyboard_t& get();

    iterator insert(iterator parent, const poly_key_handler_t& element);

    void erase(iterator position);

    bool dispatch(key_type                    virtual_key,
                  bool                        pressed,
                  modifiers_t          modifiers,
                  const any_regular_t& base_handler);

private:
    iterator handler_to_iterator(const any_regular_t& handler);

    keyboard_forest_t forest_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
// ADOBE_KEYBOARD_HPP
#endif

/****************************************************************************************************/
