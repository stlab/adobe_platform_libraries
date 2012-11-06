/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_SENTENCES_EXPRESSION_SENTENCE_HPP
#define ADOBE_SENTENCES_EXPRESSION_SENTENCE_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <list>

#include <boost/function.hpp>

#include <adobe/algorithm/for_each.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>

#include "command_system_fwd.hpp"
#include "poly_sentence.hpp"
#include "poly_subject.hpp"
#include "poly_direct_object.hpp"
#include "sentence_terminology.hpp"

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct unary_expression_sentence_t
{
    typedef adobe::dictionary_t subject_type;

    // REVISIT (fbrereto) : Get rid of this-- move it somewhere into the command system.
    typedef boost::function<void (name_t                verb,
                                  const poly_subject_t& subject,
                                  event_t               event)> monitor_proc_t;

    typedef std::list<monitor_proc_t> notifier_set_t;
    typedef notifier_set_t::iterator  connection_t;

    unary_expression_sentence_t(name_t         verb,
                                const array_t& subject_expression) :
        verb_m(verb),
        subject_expression_m(subject_expression)
    {
        if (subject_expression_m == array_t())
            throw std::runtime_error("Subject expression cannot be empty");
    }

    unary_expression_sentence_t& operator=(const unary_expression_sentence_t& rhs)
    {
        verb_m = rhs.verb_m;
        subject_expression_m = rhs.subject_expression_m;
        notifier_set_m = rhs.notifier_set_m;

        return *this;
    }

    connection_t monitor(const monitor_proc_t& f);

    void disconnect(connection_t connection)
        { notifier_set_m.erase(connection); }

    void notify(const poly_subject_t* subject, event_t event) const
    {
        adobe::for_each(notifier_set_m, boost::bind(&notifier_set_t::value_type::operator(),
                                                    _1,
                                                    verb_m,
                                                    boost::ref(*subject),
                                                    event));
    }

    bool test_subject(const subject_type& subject);

    name_t verb() const { return verb_m; }

private:
    name_t         verb_m;
    array_t        subject_expression_m;
    notifier_set_t notifier_set_m;
};

/**************************************************************************************************/

struct binary_expression_sentence_t
{
    typedef adobe::dictionary_t subject_type;
    typedef adobe::dictionary_t direct_object_type;

    // REVISIT (fbrereto) : Get rid of this-- move it somewhere into the command system.
    typedef boost::function<void (name_t                      verb,
                                  const poly_subject_t&       subject,
                                  const poly_direct_object_t& dirobj,
                                  event_t                     event)> monitor_proc_t;

    typedef std::list<monitor_proc_t> notifier_set_t;
    typedef notifier_set_t::iterator  connection_t;

    binary_expression_sentence_t(name_t         verb,
                                 const array_t& subject_expression,
                                 const array_t& dirobj_expression) :
        verb_m(verb),
        subject_expression_m(subject_expression),
        dirobj_expression_m(dirobj_expression)
    {
        if (subject_expression_m == array_t())
            throw std::runtime_error("Subject expression cannot be empty");

        if (dirobj_expression_m == array_t())
            throw std::runtime_error("Direct object expression cannot be empty");
    }

    binary_expression_sentence_t& operator=(const binary_expression_sentence_t& rhs)
    {
        verb_m = rhs.verb_m;
        subject_expression_m = rhs.subject_expression_m;
        dirobj_expression_m = rhs.dirobj_expression_m;
        notifier_set_m = rhs.notifier_set_m;

        return *this;
    }

    connection_t monitor(const monitor_proc_t& f);

    void disconnect(connection_t connection)
        { notifier_set_m.erase(connection); }

    void notify(const poly_subject_t* subject, const poly_direct_object_t* dirobj, event_t event) const
    {
        adobe::for_each(notifier_set_m, boost::bind(&notifier_set_t::value_type::operator(),
                                                    _1,
                                                    verb_m,
                                                    boost::ref(*subject),
                                                    boost::ref(*dirobj),
                                                    event));
    }

    bool test_subject(const subject_type& subject);
    bool test_direct_object(const subject_type& subject, const direct_object_type& direct_object);

    name_t verb() const { return verb_m; }

private:
    name_t         verb_m;
    array_t        subject_expression_m;
    array_t        dirobj_expression_m;
    notifier_set_t notifier_set_m;
};

/**************************************************************************************************/

poly_sentence_t make_expression_sentence(name_t             verb,
                                         const std::string& subject_requirement,
                                         const std::string& direct_object_requirement = std::string());

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_SENTENCES_EXPRESSION_SENTENCE_HPP
#endif

/**************************************************************************************************/
