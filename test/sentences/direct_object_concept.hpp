/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_DIRECT_OBJECT_CONCEPT_HPP
#define ADOBE_SENTENCES_DIRECT_OBJECT_CONCEPT_HPP

/**************************************************************************************************/

#include <boost/concept_check.hpp>
#include <boost/ref.hpp>

#include <adobe/any_regular.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

#ifdef ADOBE_HAS_CPLUS0X_CONCEPTS

/*************************************************************************************************/

auto concept DirectObjectConcept<typename DirectObject> 
//: RegularConcept<DirectObject> -- DirectObjects not yet regular
: std::CopyConstructible<DirectObject>
{
    template direct_object_type;
    direct_object_type project(DirectObject& v) const;
};

/*************************************************************************************************/

auto concept DirectObjectMFConcept<typename DirectObject> 
//: RegularConcept<DirectObject> -- DirectObjects not yet regular
: std::CopyConstructible<DirectObject>
{
    template direct_object_type = DirectObject::direct_object_type;
    direct_object_type DirectObject::project(model_type value) const; 
};

/*************************************************************************************************/

template <DirectObjectMFConcept T>
concept_map DirectObjectConcept<T>
{
    typedef DirectObjectMFConcept<T>::direct_object_type direct_object_type;

    inline direct_object_type project(T& v) const
    { return v.project(); }
};

/*************************************************************************************************/

template <DirectObjectConcept T>
concept_map DirectObjectConcept<boost::reference_wrapper<T> >
{
    typedef DirectObjectConcept<T>::direct_object_type direct_object_type;

    inline direct_object_type project(boost::reference_wrapper<T>& r) const
    { return DirectObjectConcept<T>::project(static_cast<const T&>(r)); }
};

/*************************************************************************************************/

#else

/*************************************************************************************************/
    
template <class DirectObject>
struct direct_object_type
{
    typedef typename boost::unwrap_reference<DirectObject>::type::direct_object_type type;
};

/*************************************************************************************************/
#ifndef ADOBE_SENTENCES_SUBJECT_CONCEPT_HPP
template <class DO> // DO models DirectObject
inline typename direct_object_type<DO>::type project(const DO& v)
{ return v.project(); }
#endif
/*************************************************************************************************/

template <class T>
struct DirectObjectConcept
{
    typedef typename direct_object_type<T>::type direct_object_type; 

    static direct_object_type project(const T& dirobj)
    {
        using adobe::project; // pick up default version which looks for member functions
        return project(dirobj); // unqualified to allow user versions
    }

// Concept checking:
    //use pointers since not required to be default constructible
    T*  t; 

    void constraints() {    
        // refinement of:
        // boost::function_requires<RegularConcept<T> >(); // not yet, DirectObjects not yet regular
        // boost::function_requires<boost::CopyConstructibleConcept<T> >();

        // associated types:
        typedef direct_object_type associated_type;
        
        // operations:
        using adobe::project; // pick up default version which looks for member functions
        project(*t);
    }
};

template <class T>
struct DirectObjectConcept<boost::reference_wrapper<T> > : DirectObjectConcept<T> 
{ 
    void constraints() {
        //boost concept check lib gets confused on VC8 without this
        DirectObjectConcept<T>::constraints();
    }
};

/*************************************************************************************************/
// ADOBE_HAS_CPLUS0X_CONCEPTS
#endif

/*************************************************************************************************/

} //namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_DIRECT_OBJECT_CONCEPT_HPP
#endif

/**************************************************************************************************/
