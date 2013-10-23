/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_POLY_DIRECT_OBJECT_HPP
#define ADOBE_SENTENCES_POLY_DIRECT_OBJECT_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/poly.hpp>

#include "direct_object_concept.hpp"

/**************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct poly_direct_object_interface : poly_copyable_interface
{
    virtual ~poly_direct_object_interface() { }
    virtual any_regular_t project() const = 0;
};

/*************************************************************************************************/

template <typename T>
struct poly_direct_object_instance : optimized_storage_type<T, poly_direct_object_interface>::type
{
    ADOBE_CLASS_REQUIRE(T, adobe, DirectObjectConcept);

    poly_direct_object_instance(const T& x) :
        optimized_storage_type<T, poly_direct_object_interface>::type(x) { }

    poly_direct_object_instance(poly_direct_object_instance& x, move_ctor m) :
        optimized_storage_type<T, poly_direct_object_interface>::type(x, m) {}

    any_regular_t project() const
    { return any_regular_t(DirectObjectConcept<T>::project(this->get())); }
};

/*************************************************************************************************/

struct direct_object : poly_base<poly_direct_object_interface, poly_direct_object_instance>
{
    template <typename T>
    explicit direct_object(const T& s) :
        poly_base<poly_direct_object_interface, poly_direct_object_instance>(s) { }

    /// mctor
    direct_object(direct_object& x, move_ctor m) :
        poly_base<poly_direct_object_interface, poly_direct_object_instance>(x, m) { }

    any_regular_t project() const
    { return interface_ref().project(); }
};

/*************************************************************************************************/

typedef poly<direct_object> poly_direct_object_t;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_POLY_DIRECT_OBJECT_HPP
#endif

/**************************************************************************************************/
