/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_COMMAND_SYSTEM_HPP
#define ADOBE_SENTENCES_COMMAND_SYSTEM_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#ifndef NDEBUG
    #include <iostream>
#endif
#include <list>
#include <utility>

#include <boost/function.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>

#include <adobe/algorithm/for_each_position.hpp>
#include <adobe/forest.hpp>
#include <adobe/table_index.hpp>

#include "command_system_fwd.hpp"
#include "dependency_tracker.hpp"
#include "poly_sentence.hpp"
#include "poly_subject.hpp"
#include "poly_direct_object.hpp"
#include "table.hpp"

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

typedef boost::function<void (const poly_sentence_t& sentence,
                              const poly_subject_t&  subject,
                              event_t                what)> unary_monitor_proc_t;

typedef boost::function<void (const poly_sentence_t&       sentence,
                              const poly_subject_t&        subject,
                              const poly_direct_object_t&  direct_object,
                              event_t                      what)> binary_monitor_proc_t;

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

typedef std::pair<poly_sentence_t, bool>      sentence_type;
typedef std::pair<poly_subject_t, bool>       subject_type;
typedef std::pair<poly_direct_object_t, bool> dirobj_type;

/**************************************************************************************************/

class entry_t : boost::totally_ordered<entry_t>
{
public:
    bool is_enabled() const { return last_m; }

private:
    const sentence_type* sentence_m;       // the sentence and its enable state
    const subject_type*  subject_m;        // the subject and its enable state
    const dirobj_type*   dirobj_m;         // the dirobj and its enable state
//    dependency_set_t     dependency_set_m; // the dependency set for this entry
    bool                 last_m;           // the last enabled state of this entry

    friend class command_table_t;
    friend bool operator==(const entry_t&, const entry_t&);
    friend bool operator<(const entry_t&, const entry_t&);

#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream& s, const entry_t& e);
#endif

    entry_t(const sentence_type* sentence,
            const subject_type*  subject,
            const dirobj_type*   dirobj,
            bool                 last) :
        sentence_m(sentence),
        subject_m(subject),
        dirobj_m(dirobj),
        last_m(last)
    { }
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& s, const entry_t& e);
#endif

/**************************************************************************************************/

class notify_entry_t : boost::totally_ordered<notify_entry_t>
{
private:
    typedef boost::function<void (const poly_sentence_t*      sentence,
                                  const poly_subject_t*       subject,
                                  const poly_direct_object_t* direct_object,
                                  event_t                     what)> monitor_proc_t;

    const poly_sentence_t*      sentence_m; // the sentence and its enable state
    const poly_subject_t*       subject_m;  // the subject and its enable state
    const poly_direct_object_t* dirobj_m;   // the dirobj and its enable state
    monitor_proc_t              proc_m;     // notification proc

    friend class command_table_t;
    friend bool operator==(const notify_entry_t&, const notify_entry_t&);
    friend bool operator<(const notify_entry_t&, const notify_entry_t&);

    notify_entry_t(const poly_sentence_t*      sentence,
                   const poly_subject_t*       subject,
                   const poly_direct_object_t* dirobj,
                   const monitor_proc_t&       proc) :
        sentence_m(sentence),
        subject_m(subject),
        dirobj_m(dirobj),
        proc_m(proc)
    { }
};

/**************************************************************************************************/
/*!
    This only stores entries for the set of commands that are currently valid. Whether or not they
    are enabled is whether or not all the subparts are enabled. A sentence can be valid but disabled.
    A sentence is invalid (and hence should be removed from the table) when its subject and/or dirobj
    no longer meets the requirements of the verb.
*/
class command_table_t
{
private:
    typedef table_index_traits<const sentence_type*, entry_t> index1_type;
    typedef table_index_traits<const subject_type*, entry_t>  index2_type;
    typedef table_index_traits<const dirobj_type*, entry_t>   index3_type;

    typedef table<entry_t, index1_type, index2_type, index3_type> table_t;

    typedef table_index_traits<const poly_sentence_t*, notify_entry_t>      notify_index1_type;
    typedef table_index_traits<const poly_subject_t*, notify_entry_t>       notify_index2_type;
    typedef table_index_traits<const poly_direct_object_t*, notify_entry_t> notify_index3_type;

    typedef table<notify_entry_t, notify_index1_type, notify_index2_type, notify_index3_type> notify_table_t;

public:
    command_table_t() :
        table_m(&entry_t::sentence_m, &entry_t::subject_m, &entry_t::dirobj_m),
        notify_table_m(&notify_entry_t::sentence_m, &notify_entry_t::subject_m, &notify_entry_t::dirobj_m)
    { }

    typedef table_t::iterator iterator;

    void insert(const sentence_type& sentence);
    void insert(const sentence_type& sentence, const subject_type& subject);
    void insert(const sentence_type& sentence, const subject_type& subject, const dirobj_type& dirobj);

    void erase(const sentence_type& sentence);
    void erase(const subject_type& subject);
    void erase(const dirobj_type& dirobj);

    // REVISIT (fbrereto) : These should not be public (find, either).
    void erase(iterator entry);
    void erase(iterator first, iterator last);

    iterator find(const sentence_type& sentence,
                  const subject_type*  subject = 0,
                  const dirobj_type*   dirobj = 0);

    iterator end()
        { return table_m.end(); }

    void update_enable_state(const subject_type& subject);
    void update_enable_state(const dirobj_type& dirobj);
    void update_enable_state(const sentence_type& sentence);

    void monitor(const sentence_type& sentence, const unary_monitor_proc_t& proc);
    void monitor(const sentence_type& sentence, const binary_monitor_proc_t& proc);

private:
    typedef std::pair<iterator, iterator> range_t;

    iterator find(const entry_t& entry)
    { return table_m.find(entry); }

    void update_enable_state(entry_t& entry);    

    void entry_event(const entry_t& entry, event_t what);

    void insert(const sentence_type* sentence,
                const subject_type*  subject,
                const dirobj_type*   dirobj,
                bool                 enabled);

    void insert(const poly_sentence_t*         sentence,
                const poly_subject_t*          subject,
                const poly_direct_object_t*    dirobj,
                notify_entry_t::monitor_proc_t proc);

    table_t        table_m;
    notify_table_t notify_table_m;
};

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

class command_system_t
{
public:
    typedef implementation::sentence_type sentence_type;
    typedef implementation::subject_type  subject_type;
    typedef implementation::dirobj_type   dirobj_type;

    typedef std::list<sentence_type>    sentence_set_t;
    typedef adobe::forest<subject_type> subject_tree_t;
    typedef adobe::forest<dirobj_type>  dirobj_tree_t;

    typedef sentence_set_t::iterator sentence_token_t;
    typedef subject_tree_t::iterator subject_token_t;
    typedef dirobj_tree_t::iterator  direct_object_token_t;

    typedef std::pair<subject_token_t, direct_object_token_t> noun_token_t;

    ~command_system_t();

    subject_token_t       no_parent_subject();
    direct_object_token_t no_parent_direct_object();

    template <typename T>
    subject_token_t insert_subject(subject_token_t parent, const T& subject)
    { return insert(parent, poly_subject_t(subject)); }

    template <typename T>
    noun_token_t insert_noun(subject_token_t parent, const T& noun)
    { return insert(noun_token_t(parent, no_parent_direct_object()), noun); }

    template <typename T>
    direct_object_token_t insert_direct_object(direct_object_token_t parent, const T& dirobj)
    { return insert(parent, poly_direct_object_t(dirobj)); }

    template <typename T>
    noun_token_t insert_noun(direct_object_token_t parent, const T& noun)
    { return insert(noun_token_t(no_parent_subject(), parent), noun); }

    template <typename T>
    noun_token_t insert(const noun_token_t& parent, const T& noun)
    {
        return std::make_pair(insert_subject(parent.first, noun),
                              insert_direct_object(parent.second, noun));
    }

    subject_token_t insert(subject_token_t parent, const poly_subject_t& subject);
    direct_object_token_t insert(direct_object_token_t parent, const poly_direct_object_t& dirobj);

    void enable(subject_token_t subject, bool enable);
    void update(subject_token_t subject);
    void erase(subject_token_t subject);

    void enable(direct_object_token_t dirobj, bool enable);
    void update(direct_object_token_t dirobj);
    void erase(direct_object_token_t dirobj);

    void enable(noun_token_t noun, bool make_enabled)
        { enable(noun.first, make_enabled); enable(noun.second, make_enabled); }
    void update(noun_token_t noun)
        { update(noun.first); update(noun.second); }
    void erase(noun_token_t noun)
        { erase(noun.first); erase(noun.second); }

    sentence_token_t insert(const poly_sentence_t& sentence);
    sentence_token_t insert(const poly_binary_sentence_t& sentence);

    template <typename T>
    sentence_token_t insert_sentence(const T& sentence)
    { return insert(poly_sentence_t(sentence)); }

    template <typename T>
    sentence_token_t insert_binary_sentence(const T& sentence)
    { return insert(poly_binary_sentence_t(sentence)); }

    void             enable(sentence_token_t sentence, bool enable);
    void             erase(sentence_token_t sentence);

    event_t inspect(sentence_token_t, subject_token_t);
    event_t inspect(sentence_token_t, subject_token_t, direct_object_token_t);

    event_t inspect(sentence_token_t sentence, const noun_token_t& subject)
    { return inspect(sentence, subject.first); }
    event_t inspect(sentence_token_t sentence, const noun_token_t& subject, direct_object_token_t dirobj)
    { return inspect(sentence, subject.first, dirobj); }
    event_t inspect(sentence_token_t sentence, subject_token_t subject, const noun_token_t& dirobj)
    { return inspect(sentence, subject, dirobj.second); }
    event_t inspect(sentence_token_t sentence, const noun_token_t& subject, const noun_token_t& dirobj)
    { return inspect(sentence, subject.first, dirobj.second); }

    // REVISIT (fbrereto) : When a new monitor comes on the scene for a sentence, it should be called with all
    //                      entries present in the enable_table for the sentence it wants to monitor.
    void monitor(sentence_token_t sentence, const unary_monitor_proc_t& proc);
    void monitor(sentence_token_t sentence, const binary_monitor_proc_t& proc);

private:
    typedef subject_tree_t::preorder_iterator                 subject_iterator_t;
    typedef std::pair<subject_iterator_t, subject_iterator_t> subject_range_t;

    typedef dirobj_tree_t::preorder_iterator                  dirobj_iterator_t;
    typedef std::pair<dirobj_iterator_t, dirobj_iterator_t>   dirobj_range_t;

    void sweep_for(sentence_set_t::iterator position);
    void sweep_for(subject_iterator_t       position);
    void sweep_for(dirobj_iterator_t        position);

    void sweep_for_unary_sentence(sentence_set_t::value_type& sentence);

    void sweep_for_binary_sentence(sentence_set_t::value_type& sentence);
    void sweep_for_binary_sentence_with_subject(sentence_set_t::value_type& the_sentence,
                                                subject_iterator_t          subject);
    void sweep_for_binary_sentence_with_dirobj(sentence_set_t::value_type& the_sentence,
                                               dirobj_iterator_t           dirobj);

    void unary_sentence_validation(sentence_set_t::value_type& the_sentence,
                                   subject_iterator_t          subject);
    void binary_sentence_validation(sentence_set_t::value_type& the_sentence,
                                    subject_iterator_t          subject,
                                    dirobj_iterator_t           dirobj);

    sentence_set_t                  sentence_set_m;
    subject_tree_t                  subject_tree_m;
    dirobj_tree_t                   dirobj_tree_m;
    implementation::command_table_t command_table_m;
};

/**************************************************************************************************/

template <typename T>
struct unary_notifier_adaptor
{
    typedef typename T::sentence_type            sentence_type;
    typedef typename sentence_type::subject_type subject_type;

    typedef boost::function<void (const subject_type&,
                                  adobe::event_t)> monitor_proc_type;

    explicit unary_notifier_adaptor(const monitor_proc_type& proc) :
        proc_m(proc)
    { }

    void operator()(const adobe::poly_sentence_t& /*sentence*/,
                    const adobe::poly_subject_t&  subject,
                    adobe::event_t                what) const
    {
        proc_m(subject.project().template cast<subject_type>(),
               what);
    }

    monitor_proc_type proc_m;
};

/**************************************************************************************************/

template <typename T>
inline adobe::unary_monitor_proc_t make_unary_notifier(const T& x)
{ return adobe::unary_monitor_proc_t(adobe::unary_notifier_adaptor<T>(x)); }

/**************************************************************************************************/

template <typename T>
struct binary_notifier_adaptor
{
    typedef typename T::sentence_type                  sentence_type;
    typedef typename sentence_type::subject_type       subject_type;
    typedef typename sentence_type::direct_object_type direct_object_type;

    typedef boost::function<void (const subject_type&,
                                  const direct_object_type&,
                                  adobe::event_t)> monitor_proc_type;

    explicit binary_notifier_adaptor(const monitor_proc_type& proc) :
        proc_m(proc)
    { }

    void operator()(const adobe::poly_sentence_t&       /*sentence*/,
                    const adobe::poly_subject_t&        subject,
                    const adobe::poly_direct_object_t&  direct_object,
                    adobe::event_t                      what) const
    {
        proc_m(subject.project().template cast<subject_type>(),
               direct_object.project().template cast<direct_object_type>(),
               what);
    }

    monitor_proc_type proc_m;
};

/**************************************************************************************************/

template <typename T>
inline adobe::binary_monitor_proc_t make_binary_notifier(const T& x)
{ return adobe::binary_monitor_proc_t(adobe::binary_notifier_adaptor<T>(x)); }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_COMMAND_SYSTEM_HPP
#endif

/**************************************************************************************************/
