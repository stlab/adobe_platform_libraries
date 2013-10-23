/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_SENTENCE_CONCEPT_HPP
#define ADOBE_SENTENCES_SENTENCE_CONCEPT_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/concept_check.hpp>
#include <boost/ref.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

#ifdef ADOBE_HAS_CPLUS0X_CONCEPTS

/**************************************************************************************************/

auto concept UnarySentenceConcept <typename T> 
// not yet : RegularConcept<T> 
: std::CopyConstructible<T>
{
    typename subject_type;

    bool test_subject(T& t, const subject_type& subject);
};

/**************************************************************************************************/

auto concept UnarySentenceMFConcept <typename UnarySentence>
// not yet : RegularConcept<UnarySentence>
: std::CopyConstructible<UnarySentence>
{
    typename subject_type = UnarySentence::subject_type;

    bool UnarySentence::test_subject(const subject_type& subject); 
};

/**************************************************************************************************/

template <UnarySentenceMFConcept T>
concept_map UnarySentenceConcept<T>
{
    typedef UnarySentenceMFConcept<T>::subject_type subject_type;

    bool test_subject(T& t, const subject_type& subject)
        { return t.is_doable(subject); }
};

/**************************************************************************************************/

template <UnarySentenceConcept T>
concept_map UnarySentenceConcept<boost::reference_wrapper<T> >
{
    typedef UnarySentenceConcept<T>::subject_type subject_type;

    bool test_subject(boost::reference_wrapper<T>& r, const subject_type& subject)
    {
        return UnarySentenceConcept<T>::test_subject(static_cast<T&>(r), subject);
    }
};

/**************************************************************************************************/

auto concept BinarySentenceConcept<typename UnarySentence> : UnarySentenceConcept<UnarySentence> 
{
    typename direct_object_type;

    bool test_direct_object(UnarySentence& s, const subject_type& subject, const direct_object_type& dirobj);
};

/**************************************************************************************************/

auto concept BinarySentenceMFConcept<typename UnarySentence> : UnarySentenceMFConcept<UnarySentence>
{
    typename direct_object_type = BinarySentenceMFConcept::direct_object_type;

    bool UnarySentence::test_direct_object(const subject_type& subject, const direct_object_type& dirobj); 
};

/**************************************************************************************************/

template <typename T>
concept_map BinarySentenceConcept<BinarySentenceMFConcept<T> >
{
    typename subject_type = BinarySentenceMFConcept<T>::subject_type;
    typename direct_object_type = BinarySentenceMFConcept<T>::direct_object_type;

    bool test_subject(BinarySentenceMFConcept<T>& t, const subject_type& subject)
        { return t.test_subject(subject); }

    bool test_direct_object(BinarySentenceMFConcept<T>& t, const subject_type& subject, const direct_object_type& dirobj)
        { return t.test_direct_object(subject, dirobj); }
};

/**************************************************************************************************/

template <typename T>
concept_map BinarySentenceConcept<boost::reference_wrapper<BinarySentenceConcept<T> > >
{
    typename subject_type = BinarySentenceConcept<T>::subject_type;
    typename direct_object_type = BinarySentenceConcept<T>::direct_object_type;

    bool test_subject(boost::reference_wrapper<BinarySentenceConcept<T> >& r, 
                      const subject_type&                                  subject)
    { 
        return BinarySentenceConcept<BinarySentenceConcept<T> >::test_subject(
            *r.get_pointer(), subject); 
    }

    void test_direct_object(boost::reference_wrapper<BinarySentenceConcept<T> >& r, 
                            const subject_type&                                  subject,
                            const direct_object_type&                            dirobj)
    { 
        return BinarySentenceConcept<BinarySentenceConcept<T> >::test_direct_object(
            *r.get_pointer(), subject, dirobj); 
    }
};

/**************************************************************************************************/

#else

/*************************************************************************************************/
#ifndef ADOBE_SENTENCES_SUBJECT_CONCEPT_HPP
template <class Sentence>
struct subject_type
{
    typedef typename boost::unwrap_reference<Sentence>::type::subject_type type;
};
#endif
/**************************************************************************************************/

template <class T>
inline bool test_subject(T& t, const typename subject_type<T>::type& subject)
{ return t.test_subject(subject); }

/**************************************************************************************************/

template <class T>
struct UnarySentenceConcept
{
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef typename subject_type<T>::type subject_type; 

    static bool test_subject(T& t, const subject_type& subject)
    {
        using adobe::test_subject; // pick up default version which looks for member functions
        return test_subject(t, subject); // unqualified to allow user versions
    }

    // Concept checking:
    void constraints()
    {
        // not yet: boost::function_requires<RegularConcept<UnarySentence> >();
        // boost::function_requires<boost::CopyConstructibleConcept<UnarySentence> >();

        using adobe::test_subject; 
        foo = test_subject(*UnarySentence, *subject);
    }

    //use pointers since not required to be default constructible
    T*                  UnarySentence;
    const subject_type* subject;
    bool                foo;
#endif
};

template <class T>
struct UnarySentenceConcept<T*> : public UnarySentenceConcept<T>
{
    typedef typename UnarySentenceConcept<T>::subject_type subject_type;

    static bool test_subject(T* r, const subject_type& subject) 
    { return UnarySentenceConcept<T>::test_subject(*r, subject); }

#if !defined(ADOBE_NO_DOCUMENTATION)
    void constraints()
    {
        //boost concept check lib gets confused on VC8 without this
        UnarySentenceConcept<T>::constraints();
    }
#endif
};

/*************************************************************************************************/
#ifndef ADOBE_SENTENCES_DIRECT_OBJECT_CONCEPT_HPP
template <class Sentence>
struct direct_object_type
{
    typedef typename boost::unwrap_reference<Sentence>::type::direct_object_type type;
};
#endif
/**************************************************************************************************/

template <class T>
inline bool test_direct_object(T&                                          t,
                               const typename subject_type<T>::type&       subject,
                               const typename direct_object_type<T>::type& dirobj)
{ return t.test_direct_object(subject, dirobj); }

/**************************************************************************************************/

template <class T>
struct BinarySentenceConcept : UnarySentenceConcept<T>
{
#if ! defined(ADOBE_NO_DOCUMENTATION)
    typedef typename UnarySentenceConcept<T>::subject_type subject_type; 
    typedef typename direct_object_type<T>::type           direct_object_type; 

    static bool test_direct_object(T& t, const subject_type& subject, const direct_object_type& dirobj)
    {
        using adobe::test_direct_object;
        return test_direct_object(t, subject, dirobj);
    }

    // Concept checking:
    void constraints() {
        // not yet: boost::function_requires<RegularConcept<T> >();
        //boost::function_requires<boost::CopyConstructibleConcept<T> >();  

        using adobe::test_subject;
        this->foo = test_subject(*t2, *(this->subject)); 

        using adobe::test_direct_object;
        this->foo = test_direct_object(*t2, *(this->subject), *dirobj); 
    }

    //use pointers since not required to be default constructible
    T*                        t2;
    const direct_object_type* dirobj;
#endif

};

template <class T>
struct BinarySentenceConcept<T*> : BinarySentenceConcept<T> 
{
    typedef typename BinarySentenceConcept<T>::subject_type       subject_type;
    typedef typename BinarySentenceConcept<T>::direct_object_type direct_object_type;

    static bool test_subject(T* r, const subject_type& subject)
    { 
        return BinarySentenceConcept<T>::test_subject(*r, subject); 
    }

    static void test_direct_object(T* r, const subject_type& subject, const direct_object_type& dirobj)
    { 
        return BinarySentenceConcept<T>::test_direct_object(*r, dirobj); 
    }

#if !defined(ADOBE_NO_DOCUMENTATION)
    void constraints() {
        //boost concept check lib gets confused on VC8 without this
        BinarySentenceConcept<T>::constraints();
    }
#endif
};

/**************************************************************************************************/
// ADOBE_HAS_CPLUS0X_CONCEPTS
#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_SENTENCE_CONCEPT_HPP
#endif

/**************************************************************************************************/
