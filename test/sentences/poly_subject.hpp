/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_POLY_SUBJECT_HPP
#define ADOBE_SENTENCES_POLY_SUBJECT_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/poly.hpp>

#include "subject_concept.hpp"

/**************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct poly_subject_interface : poly_copyable_interface
{
    virtual ~poly_subject_interface() { }
    virtual any_regular_t project() const = 0;
};

/*************************************************************************************************/

template <typename T>
struct poly_subject_instance : optimized_storage_type<T, poly_subject_interface>::type
{
    ADOBE_CLASS_REQUIRE(T, adobe, SubjectConcept);

    poly_subject_instance(const T& x) :
        optimized_storage_type<T, poly_subject_interface>::type(x) { }

    poly_subject_instance(poly_subject_instance& x, move_ctor m) :
        optimized_storage_type<T, poly_subject_interface>::type(x, m) { }

    any_regular_t project() const
    {
        return any_regular_t(SubjectConcept<T>::project(this->get()));
    }
};

/*************************************************************************************************/

struct subject : poly_base<poly_subject_interface, poly_subject_instance>
{
    template <typename T>
    explicit subject(const T& s) :
        poly_base<poly_subject_interface, poly_subject_instance>(s) { }

    /// mctor
    subject(subject& x, move_ctor m) :
        poly_base<poly_subject_interface, poly_subject_instance>(x, m) { }

    any_regular_t project() const
    { return interface_ref().project(); }
};

/*************************************************************************************************/

typedef poly<subject> poly_subject_t;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_POLY_SUBJECT_HPP
#endif

/**************************************************************************************************/
