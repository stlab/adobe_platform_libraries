/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_SUBJECT_CONCEPT_HPP
#define ADOBE_SENTENCES_SUBJECT_CONCEPT_HPP

/**************************************************************************************************/

#include <boost/concept_check.hpp>
#include <boost/ref.hpp>

#include <adobe/any_regular.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

#ifdef ADOBE_HAS_CPLUS0X_CONCEPTS

/*************************************************************************************************/

auto concept SubjectConcept<typename Subject> 
//: RegularConcept<Subject> -- Subjects not yet regular
: std::CopyConstructible<Subject>
{
    template subject_type;
    subject_type project(Subject& v) const;
};

/*************************************************************************************************/

auto concept SubjectMFConcept<typename Subject> 
//: RegularConcept<Subject> -- Subjects not yet regular
: std::CopyConstructible<Subject>
{
    template subject_type = Subject::subject_type;
    subject_type Subject::project(model_type value) const; 
};

/*************************************************************************************************/

template <SubjectMFConcept T>
concept_map SubjectConcept<T>
{
    typedef SubjectMFConcept<T>::subject_type subject_type;

    inline subject_type project(T& v) const
    { return v.project(); }
};

/*************************************************************************************************/

template <SubjectConcept T>
concept_map SubjectConcept<boost::reference_wrapper<T> >
{
    typedef SubjectConcept<T>::subject_type subject_type;

    inline subject_type project(boost::reference_wrapper<T>& r) const
    { return SubjectConcept<T>::project(static_cast<const T&>(r)); }
};

/*************************************************************************************************/

#else

/*************************************************************************************************/
    
template <class Subject>
struct subject_type
{
    typedef typename boost::unwrap_reference<Subject>::type::subject_type type;
};

/*************************************************************************************************/
#ifndef ADOBE_SENTENCES_DIRECT_OBJECT_CONCEPT_HPP
template <class S> // S models Subject
inline typename subject_type<S>::type project(const S& v)
{ return v.project(); }
#endif
/*************************************************************************************************/

template <class T>
struct SubjectConcept
{
    typedef typename subject_type<T>::type subject_type; 

    static subject_type project(const T& subject)
    {
        using adobe::project; // pick up default version which looks for member functions
        return project(subject); // unqualified to allow user versions
    }

// Concept checking:
    //use pointers since not required to be default constructible
    T*  t; 

    void constraints() {    
        // refinement of:
        // boost::function_requires<RegularConcept<T> >(); // not yet, Subjects not yet regular
        // boost::function_requires<boost::CopyConstructibleConcept<T> >();

        // associated types:
        typedef subject_type associated_type;
        
        // operations:
        using adobe::project; // pick up default version which looks for member functions
        project(*t);
    }
};

template <class T>
struct SubjectConcept<boost::reference_wrapper<T> > : SubjectConcept<T> 
{ 
    void constraints() {
        //boost concept check lib gets confused on VC8 without this
        SubjectConcept<T>::constraints();
    }
};

/*************************************************************************************************/
// ADOBE_HAS_CPLUS0X_CONCEPTS
#endif

/*************************************************************************************************/

} //namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_SUBJECT_CONCEPT_HPP
#endif

/**************************************************************************************************/
