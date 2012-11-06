/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include "command_system.hpp"

#include <adobe/algorithm/find.hpp>
#include <adobe/algorithm/for_each.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

struct unary_monitor_adaptor_t
{
    typedef void result_type;

    explicit unary_monitor_adaptor_t(const adobe::unary_monitor_proc_t& proc) :
        proc_m(proc)
    { }

    void operator()(const adobe::poly_sentence_t*      sentence,
                    const adobe::poly_subject_t*       subject,
                    const adobe::poly_direct_object_t* /*direct_object*/,
                    adobe::event_t                     what)
    {
        assert(sentence);
        assert(subject);

        proc_m(*sentence, *subject, what);
    }

    adobe::unary_monitor_proc_t proc_m;
};

/**************************************************************************************************/

struct binary_monitor_adaptor_t
{
    typedef void result_type;

    explicit binary_monitor_adaptor_t(const adobe::binary_monitor_proc_t& proc) :
        proc_m(proc)
    { }

    void operator()(const adobe::poly_sentence_t*      sentence,
                    const adobe::poly_subject_t*       subject,
                    const adobe::poly_direct_object_t* direct_object,
                    adobe::event_t                     what)
    {
        assert(sentence);
        assert(subject);
        assert(direct_object);

        proc_m(*sentence, *subject, *direct_object, what);
    }

    adobe::binary_monitor_proc_t proc_m;
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

command_system_t::~command_system_t()
{
    // erase all sentences? (and subjects? (and objects?))

    while (true)
    {
        command_system_t::sentence_set_t::iterator iter(sentence_set_m.begin());

        if (iter == sentence_set_m.end())
            break;

        erase(iter);
    }
}

/**************************************************************************************************/

command_system_t::subject_token_t command_system_t::no_parent_subject()
{
    return subject_tree_m.begin();
}

/**************************************************************************************************/

command_system_t::subject_token_t command_system_t::insert(subject_token_t       parent,
                                                           const poly_subject_t& subject)
{
    command_system_t::subject_token_t result =
        subject_tree_m.insert(parent, subject_type(subject, true));

    sweep_for(subject_iterator_t(result));

    return result;
}

/**************************************************************************************************/

void command_system_t::enable(subject_token_t subject, bool enable)
{
	subject->second = enable;

	command_table_m.update_enable_state(*subject);
}

/**************************************************************************************************/

void command_system_t::update(subject_token_t subject)
{
    //... do a sweep for all entries with this subject as a dependency

    sweep_for(subject_iterator_t(subject));
}

/**************************************************************************************************/

void command_system_t::erase(subject_token_t subject)
{
    // REVISIT: delete all the children subjects of this subject

    command_table_m.erase(*subject);

	subject_tree_m.erase(subject);
}

/**************************************************************************************************/

command_system_t::direct_object_token_t command_system_t::no_parent_direct_object()
{
	return dirobj_tree_m.begin();
}

/**************************************************************************************************/

command_system_t::direct_object_token_t
command_system_t::insert(direct_object_token_t parent, const poly_direct_object_t& dirobj)
{
    command_system_t::direct_object_token_t result =
        dirobj_tree_m.insert(parent, dirobj_type(dirobj, true));

    sweep_for(dirobj_iterator_t(result));

    return result;
}

/**************************************************************************************************/

void command_system_t::enable(direct_object_token_t dirobj, bool enable)
{
	dirobj->second = enable;

	command_table_m.update_enable_state(*dirobj);
}

/**************************************************************************************************/

void command_system_t::update(direct_object_token_t dirobj)
{
    //... do a sweep for all entries with this dirobj as a dependency

    sweep_for(dirobj_iterator_t(dirobj));
}

/**************************************************************************************************/

void command_system_t::erase(direct_object_token_t dirobj)
{
    // REVISIT: delete all the children dirobjs of this dirobj

    command_table_m.erase(*dirobj);

	dirobj_tree_m.erase(dirobj);
}

/**************************************************************************************************/

command_system_t::sentence_token_t command_system_t::insert(const poly_sentence_t& sentence)
{
	sentence_set_m.push_front(sentence_type(sentence, true));

    sweep_for(sentence_set_m.begin());

	return sentence_set_m.begin();
}

/**************************************************************************************************/

command_system_t::sentence_token_t command_system_t::insert(const poly_binary_sentence_t& sentence)
{
	return insert(poly_cast<const poly_sentence_t&>(sentence));
}

/**************************************************************************************************/

void command_system_t::enable(sentence_token_t sentence, bool enable)
{
	sentence->second = enable;

	command_table_m.update_enable_state(*sentence);
}

/**************************************************************************************************/

void command_system_t::erase(sentence_token_t sentence)
{
    command_table_m.erase(*sentence);

	sentence_set_m.erase(sentence);
}

/**************************************************************************************************/

event_t command_system_t::inspect(sentence_token_t sentence_pos,
                                  subject_token_t  subject_pos)
{
    const subject_type* subject_type_ptr(&(*subject_pos));

    implementation::command_table_t::iterator pos =
        command_table_m.find(*sentence_pos, subject_type_ptr);

    return pos == command_table_m.end() ? remove_k :
                                         pos->is_enabled() ? (add_k | enable_k) :
                                                             (add_k | disable_k);
}

/**************************************************************************************************/

event_t command_system_t::inspect(sentence_token_t      sentence_pos,
                                  subject_token_t       subject_pos,
                                  direct_object_token_t dirobj_pos)
{
    const subject_type* subject_type_ptr(&(*subject_pos));
    const dirobj_type*  dirobj_type_ptr(&(*dirobj_pos));

    implementation::command_table_t::iterator pos =
        command_table_m.find(*sentence_pos, subject_type_ptr, dirobj_type_ptr);

    return pos == command_table_m.end() ? remove_k :
                                         pos->is_enabled() ? (add_k | enable_k) :
                                                             (add_k | disable_k);
}

/**************************************************************************************************/

void command_system_t::sweep_for(sentence_set_t::iterator position)
{
    // brute force sweep on this sentence for all subjects and dirobjs.

    sentence_set_t::value_type& the_sentence(*position);
    poly_sentence_t&            sentence(the_sentence.first);

    if (poly_cast<poly_binary_sentence_t*>(&sentence))
        { sweep_for_binary_sentence(the_sentence); }
    else
        { sweep_for_unary_sentence(the_sentence); }
}

/**************************************************************************************************/

void command_system_t::sweep_for(subject_iterator_t position)
{
    sentence_set_t::iterator iter(sentence_set_m.begin());
    sentence_set_t::iterator last(sentence_set_m.end());

    while (iter != last)
    {
        if (poly_cast<poly_binary_sentence_t*>(&iter->first))
            { sweep_for_binary_sentence_with_subject(*iter, position); }
        else
            { unary_sentence_validation(*iter, position); }

        ++iter;
    }
}

/**************************************************************************************************/

void command_system_t::sweep_for(dirobj_iterator_t position)
{
    sentence_set_t::iterator iter(sentence_set_m.begin());
    sentence_set_t::iterator last(sentence_set_m.end());

    while (iter != last)
    {
        if (poly_cast<poly_binary_sentence_t*>(&iter->first))
            sweep_for_binary_sentence_with_dirobj(*iter, position);

        ++iter;
    }
}

/**************************************************************************************************/

void command_system_t::sweep_for_unary_sentence(sentence_set_t::value_type& the_sentence)
{
    adobe::for_each_position(preorder_range(subject_tree_m),
                             boost::bind(&command_system_t::unary_sentence_validation,
                                         boost::ref(*this),
                                         boost::ref(the_sentence),
                                         _1));
}

/**************************************************************************************************/

void command_system_t::sweep_for_binary_sentence(sentence_set_t::value_type& the_sentence)
{
    adobe::for_each_position(preorder_range(subject_tree_m),
                             boost::bind(&command_system_t::sweep_for_binary_sentence_with_subject,
                                         boost::ref(*this),
                                         boost::ref(the_sentence),
                                         _1));
}

/**************************************************************************************************/

void command_system_t::sweep_for_binary_sentence_with_subject(sentence_set_t::value_type& the_sentence,
                                                              subject_iterator_t          subject)
{
    adobe::for_each_position(preorder_range(dirobj_tree_m),
                             boost::bind(&command_system_t::binary_sentence_validation,
                                         boost::ref(*this),
                                         boost::ref(the_sentence),
                                         subject,
                                         _1));
}

/**************************************************************************************************/

void command_system_t::sweep_for_binary_sentence_with_dirobj(sentence_set_t::value_type& the_sentence,
                                                             dirobj_iterator_t           dirobj)
{
    adobe::for_each_position(preorder_range(subject_tree_m),
                             boost::bind(&command_system_t::binary_sentence_validation,
                                         boost::ref(*this),
                                         boost::ref(the_sentence),
                                         _1,
                                         dirobj));
}

/**************************************************************************************************/

void command_system_t::unary_sentence_validation(sentence_set_t::value_type& the_sentence,
                                                 subject_iterator_t          subject)
{
    poly_sentence_t&    sentence(the_sentence.first);
    const subject_type* subject_type_ptr(&(*subject));

    implementation::command_table_t::iterator pos =
        command_table_m.find(the_sentence, subject_type_ptr);

    bool subject_success(sentence.test_subject(subject_type_ptr->first));

    if (!subject_success) // this sentence and subject are incompatible.
        command_table_m.erase(pos);
    else if (pos == command_table_m.end()) // compatible and an entry need be added
        command_table_m.insert(the_sentence, *subject);
    else // compatible and entry exists. Since enabling is handled elsewhere do nothing.
        { }
}

/**************************************************************************************************/

void command_system_t::binary_sentence_validation(sentence_set_t::value_type& the_sentence,
                                                  subject_iterator_t          subject,
                                                  dirobj_iterator_t           dirobj)
{
    poly_sentence_t&        unary_sentence(the_sentence.first);
    poly_binary_sentence_t& sentence(*poly_cast<poly_binary_sentence_t*>(&unary_sentence));
    const subject_type*     subject_type_ptr(&(*subject));
    const dirobj_type*      dirobj_type_ptr(&(*dirobj));

    implementation::command_table_t::iterator pos =
        command_table_m.find(the_sentence, subject_type_ptr, dirobj_type_ptr);

    bool subject_success(sentence.test_subject(subject_type_ptr->first));
    bool dirobj_success(subject_success &&
                        sentence.test_direct_object(subject_type_ptr->first, dirobj_type_ptr->first));

    if (!(subject_success && dirobj_success)) // this sentence, subject and dirobj are incompatible.
        command_table_m.erase(pos);
    else if (pos == command_table_m.end()) // compatible and an entry need be added
        command_table_m.insert(the_sentence, *subject, *dirobj);
    else // compatible and entry exists. Since enabling is handled elsewhere do nothing.
        { }
}

/**************************************************************************************************/

void command_system_t::monitor(sentence_token_t sentence, const unary_monitor_proc_t& proc)
{
    command_table_m.monitor(*sentence, proc);
}

/**************************************************************************************************/

void command_system_t::monitor(sentence_token_t sentence, const binary_monitor_proc_t& proc)
{
    command_table_m.monitor(*sentence, proc);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

void command_table_t::insert(const sentence_type& sentence)
{
    insert(&sentence, 0, 0, sentence.second);
}

/**************************************************************************************************/

void command_table_t::insert(const sentence_type& sentence,
                            const subject_type&  subject)
{
    insert(&sentence, &subject, 0, sentence.second && subject.second);
}

/**************************************************************************************************/

void command_table_t::insert(const sentence_type& sentence,
                            const subject_type&  subject,
                            const dirobj_type&   dirobj)
{
    insert(&sentence, &subject, &dirobj, sentence.second && subject.second && dirobj.second);
}

/**************************************************************************************************/

void command_table_t::erase(const sentence_type& sentence)
{
    range_t range(table_m.equal_range1(&sentence));

    erase(range.first, range.second);
}

/**************************************************************************************************/

void command_table_t::erase(const subject_type& subject)
{
    range_t range(table_m.equal_range2(&subject));

    erase(range.first, range.second);
}

/**************************************************************************************************/

void command_table_t::erase(const dirobj_type& dirobj)
{
    range_t range(table_m.equal_range3(&dirobj));

    erase(range.first, range.second);
}

/**************************************************************************************************/

void command_table_t::erase(iterator first, iterator last)
{
    std::for_each(first, last, boost::bind(&command_table_t::entry_event,
                                           boost::ref(*this), _1, remove_k));

    table_m.erase(first, last);
}

/**************************************************************************************************/

void command_table_t::erase(iterator entry)
{
    if (entry == table_m.end())
        return;

    entry_event(*entry, remove_k);

    table_m.erase(entry);
}

/**************************************************************************************************/

command_table_t::iterator command_table_t::find(const sentence_type& sentence,
                                              const subject_type*  subject,
                                              const dirobj_type*   dirobj)
{
    return table_m.find(entry_t(&sentence, subject, dirobj, true));
}

/**************************************************************************************************/

void command_table_t::update_enable_state(const sentence_type& sentence)
{
    range_t range(table_m.equal_range1(&sentence));

    typedef void (command_table_t::*update_entry_proc_t)(entry_t& entry);

    update_entry_proc_t proc(&command_table_t::update_enable_state);

    adobe::for_each(range, boost::bind(proc, boost::ref(*this), _1));
}

/**************************************************************************************************/

void command_table_t::update_enable_state(const subject_type& subject)
{
    range_t range(table_m.equal_range2(&subject));

    typedef void (command_table_t::*update_entry_proc_t)(entry_t& entry);

    update_entry_proc_t proc(&command_table_t::update_enable_state);

    adobe::for_each(range, boost::bind(proc, boost::ref(*this), _1));
}

/**************************************************************************************************/

void command_table_t::update_enable_state(const dirobj_type& dirobj)
{
    range_t range(table_m.equal_range3(&dirobj));

    typedef void (command_table_t::*update_entry_proc_t)(entry_t& entry);

    update_entry_proc_t proc(&command_table_t::update_enable_state);

    adobe::for_each(range, boost::bind(proc, boost::ref(*this), _1));
}

/**************************************************************************************************/

void command_table_t::update_enable_state(entry_t& entry)
{
    bool sentence_enabled(entry.sentence_m != 0 && entry.sentence_m->second);
    bool subject_enabled(entry.subject_m == 0 || entry.subject_m->second);
    bool dirobj_enabled(entry.dirobj_m == 0 || entry.dirobj_m->second);

    bool instance_enabled(sentence_enabled && subject_enabled && dirobj_enabled);

    if (entry.last_m == instance_enabled)
        return;

    entry.last_m = instance_enabled;

    entry_event(entry, instance_enabled ? enable_k : disable_k);
}

/**************************************************************************************************/

void command_table_t::entry_event(const entry_t& entry, event_t what)
{
    assert(entry.sentence_m);

    const adobe::poly_sentence_t*      sentence(&entry.sentence_m->first);
    const adobe::poly_subject_t*       subject(entry.subject_m ? &entry.subject_m->first : 0);
    const adobe::poly_direct_object_t* dirobj(entry.dirobj_m ? &entry.dirobj_m->first : 0);

    notify_entry_t           dummy(sentence, 0, 0, notify_entry_t::monitor_proc_t());
    notify_table_t::iterator found(notify_table_m.find(dummy));

    if (found != notify_table_m.end())
        found->proc_m(sentence, subject, dirobj, what);
}

/**************************************************************************************************/

void command_table_t::insert(const sentence_type* sentence,
                            const subject_type*  subject,
                            const dirobj_type*   dirobj,
                            bool                 enabled)
{
    iterator pos(table_m.insert(entry_t(sentence, subject, dirobj, enabled)));
    event_t  what(add_k);

    what |= enabled ? enable_k : disable_k;

    entry_event(*pos, what);
}

/**************************************************************************************************/

void command_table_t::insert(const poly_sentence_t*         sentence,
                            const poly_subject_t*          subject,
                            const poly_direct_object_t*    dirobj,
                            notify_entry_t::monitor_proc_t proc)
{
    notify_table_m.insert(notify_entry_t(sentence, subject, dirobj, proc));
}

/**************************************************************************************************/

void command_table_t::monitor(const sentence_type& sentence, const unary_monitor_proc_t& proc)
{
    const poly_sentence_t* poly_sentence(&sentence.first);

    notify_entry_t entry(poly_sentence, 0, 0, unary_monitor_adaptor_t(proc));

    notify_table_m.insert(entry);
}

/**************************************************************************************************/

void command_table_t::monitor(const sentence_type& sentence, const binary_monitor_proc_t& proc)
{
    const poly_sentence_t* poly_sentence(&sentence.first);

    notify_entry_t entry(poly_sentence, 0, 0, binary_monitor_adaptor_t(proc));

    notify_table_m.insert(entry);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

bool operator==(const entry_t& x, const entry_t& y)
{
    // note we do not test the nonessential parts in the comparison.
    return x.sentence_m == y.sentence_m &&
           x.subject_m == y.subject_m &&
           x.dirobj_m == y.dirobj_m;
}

/**************************************************************************************************/

bool operator<(const entry_t& x, const entry_t& y)
{
    // note we do not test the nonessential parts in the comparison.

    if (x.sentence_m < y.sentence_m)
        return true;
    else if (x.sentence_m == y.sentence_m)
    {
        if (x.subject_m < y.subject_m)
            return true;
        else if (x.subject_m == y.subject_m)
            return x.dirobj_m < y.dirobj_m;

        return false;
    }

    return false;
}

/**************************************************************************************************/
#ifndef NDEBUG
std::ostream& operator<<(std::ostream& s, const entry_t& e)
{
    s << "      >> ";

    s << "verb: " << reinterpret_cast<const void*>(&e.sentence_m->first);

    if (e.subject_m != 0)
        s << "; subject: " << reinterpret_cast<const void*>(&e.subject_m->first);

    if (e.dirobj_m != 0)
        s << "; dirobj: " << reinterpret_cast<const void*>(&e.dirobj_m->first);

    s << "; enabled: " << std::boolalpha << e.last_m;

    return s;
}
#endif
/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

bool operator==(const notify_entry_t& x, const notify_entry_t& y)
{
    // note we do not test the nonessential parts in the comparison.
    return x.sentence_m == y.sentence_m &&
           x.subject_m == y.subject_m &&
           x.dirobj_m == y.dirobj_m;
}

/**************************************************************************************************/

bool operator<(const notify_entry_t& x, const notify_entry_t& y)
{
    // note we do not test the nonessential parts in the comparison.

    if (x.sentence_m < y.sentence_m)
        return true;
    else if (x.sentence_m == y.sentence_m)
    {
        if (x.subject_m < y.subject_m)
            return true;
        else if (x.subject_m == y.subject_m)
            return x.dirobj_m < y.dirobj_m;

        return false;
    }

    return false;
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
