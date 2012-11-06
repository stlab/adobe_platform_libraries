/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_POLY_SENTENCE_HPP
#define ADOBE_SENTENCES_POLY_SENTENCE_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/ref.hpp>

#include <adobe/poly.hpp>

#include "poly_subject.hpp"
#include "poly_direct_object.hpp"
#include "sentence_concept.hpp"

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\brief Pure virtual interface for the poly<unary_sentence> modeling
\ref adobe::UnarySentenceConcept
*/
struct poly_sentence_interface : poly_copyable_interface
{
    virtual ~poly_sentence_interface() { }
    virtual bool test_subject(const poly_subject_t& subject) = 0;
};

/*************************************************************************************************/
/*!
\brief Implementation of virtual interface for the poly<unary_sentence> modeling
\ref adobe::UnarySentenceConcept 
*/
template <typename T>
ADOBE_REQUIRES(adobe::UnarySentenceConcept<T>)
struct poly_sentence_instance : optimized_storage_type<T, poly_sentence_interface>::type
{
    ADOBE_CLASS_REQUIRE(T, adobe, UnarySentenceConcept);

    poly_sentence_instance(const T& x) :
        optimized_storage_type<T, poly_sentence_interface>::type(x) { }
    poly_sentence_instance(poly_sentence_instance& x, move_ctor m) :
        optimized_storage_type<T, poly_sentence_interface>::type(x, m) { }

    bool test_subject(const poly_subject_t& subject)
    {
        typedef typename UnarySentenceConcept<T>::subject_type subject_type;

        any_regular_t projection(subject.project());

        return projection.type_info() != adobe::type_info<subject_type>() ? false :
            UnarySentenceConcept<T>::test_subject(this->get(), projection.cast<subject_type>());
    }
};

/*************************************************************************************************/
/*!
\brief Representative of \ref adobe::UnarySentenceConcept so that UnarySentence
models \ref adobe::UnarySentenceConcept when T does.
*/
struct unary_sentence : public poly_base<poly_sentence_interface, poly_sentence_instance>
{
    template <typename T>
    explicit unary_sentence(const T& s) :
        poly_base<poly_sentence_interface, poly_sentence_instance>(s) { }

    /// mctor
    unary_sentence(unary_sentence& x, move_ctor m) :
        poly_base<poly_sentence_interface, poly_sentence_instance>(x, m) { }

    template <typename V>
    bool test_subject(const V& subject)
    { return interface_ref().test_subject(poly_subject_t(subject)); }
};

/*************************************************************************************************/
/*!
\brief convenience typedef.
\sa adobe::UnarySentenceConcept.
*/
typedef poly<unary_sentence> poly_sentence_t;

/*************************************************************************************************/
/*!
\brief Pure virtual interface for poly<binary_sentence> modeling
\ref adobe::BinarySentenceConcept
*/
struct poly_binary_sentence_interface : public poly_sentence_interface
{
    virtual bool test_direct_object(const poly_subject_t& subject, const poly_direct_object_t& dirobj) = 0;
};

/*************************************************************************************************/
/*!
\brief Implementation of virtual interface for the poly<binary_sentence> modeling
\ref adobe::BinarySentenceConcept 
*/
template <typename T>
ADOBE_REQUIRES(BinarySentenceConcept<T>)
struct poly_binary_sentence_instance : optimized_storage_type<T, poly_binary_sentence_interface>::type
{
    ADOBE_CLASS_REQUIRE(T, adobe, BinarySentenceConcept);

    poly_binary_sentence_instance(const T& x) :
        optimized_storage_type<T, poly_binary_sentence_interface>::type(x) { }

    poly_binary_sentence_instance(poly_binary_sentence_instance& x, move_ctor m) :
        optimized_storage_type<T, poly_binary_sentence_interface>::type(x, m) { }

    bool test_subject(const poly_subject_t& subject)
    {
        typedef typename BinarySentenceConcept<T>::subject_type subject_type;

        any_regular_t projection(subject.project());

        return projection.type_info() != adobe::type_info<subject_type>() ? false :
            BinarySentenceConcept<T>::test_subject(this->get(), projection.cast<subject_type>());
    }

    bool test_direct_object(const poly_subject_t& subject, const poly_direct_object_t& dirobj)
    {
        typedef typename BinarySentenceConcept<T>::subject_type       subject_type;
        typedef typename BinarySentenceConcept<T>::direct_object_type direct_object_type;

        any_regular_t s_projection(subject.project());
        any_regular_t do_projection(dirobj.project());

        return s_projection.type_info() != adobe::type_info<subject_type>() ||
               do_projection.type_info() != adobe::type_info<direct_object_type>() ?
            false :
            BinarySentenceConcept<T>::test_direct_object(this->get(),
                                                         s_projection.cast<subject_type>(),
                                                         do_projection.cast<direct_object_type>());
    }
};

/*************************************************************************************************/

struct binary_sentence
    : public poly_base<poly_binary_sentence_interface, poly_binary_sentence_instance>
{
    template <typename T>
    explicit binary_sentence(const T& s) :
        poly_base<poly_binary_sentence_interface, poly_binary_sentence_instance>(s) { }

    /// mctor
    binary_sentence(binary_sentence& x, move_ctor m) :
        poly_base<poly_binary_sentence_interface, poly_binary_sentence_instance>(x, m) { }

    template <typename V>
    bool test_subject(const V& subject)
    { return interface_ref().test_subject(poly_subject_t(subject)); }

    template <typename U, typename V>
    bool test_direct_object(const U& subject, const V& dirobj)
    { return interface_ref().test_direct_object(poly_subject_t(subject), poly_direct_object_t(dirobj)); }
};

/*************************************************************************************************/
/*!
\brief convenience typedef.
\sa adobe::BinarySentenceConcept .
*/
typedef poly<binary_sentence> poly_binary_sentence_t;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_POLY_SENTENCE_HPP
#endif

/**************************************************************************************************/
