/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#define BOOST_TEST_MAIN

#include <adobe/config.hpp>

#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>

#if ADOBE_PLATFORM_WIN
    #pragma warning ( push )
    #pragma warning ( disable : 4535 ) // calling XYZ requires /EHa
#endif
#include <boost/test/included/unit_test.hpp>
#if ADOBE_PLATFORM_WIN
    #pragma warning ( pop )
#endif

#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>

#include "command_system.hpp"
#include "expression_sentence.hpp"

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

struct dictionary_sentence_thing_t
{
public:
    typedef adobe::dictionary_t subject_type;
    typedef adobe::dictionary_t direct_object_type;

    dictionary_sentence_thing_t(bool has, adobe::name_t type) :
        has_m(has),
        type_m(type)
    { }

    adobe::dictionary_t project() const
    {
        adobe::dictionary_t result;

        if (type_m != adobe::name_t())
            result.insert(std::make_pair(adobe::static_name_t("type"), type_m));

        result.insert(std::make_pair(adobe::receives_k, adobe::name_t("raster_mask")));
        result.insert(std::make_pair(adobe::emits_k, adobe::name_t("raster_mask")));
        result.insert(std::make_pair(adobe::name_t("has_user_mask"), has_m));

        return result;
    }

    bool          has_m;
    adobe::name_t type_m;
};

/**************************************************************************************************/

inline const char* event_to_char(adobe::event_t what)
{
    if (what == adobe::add_k)
        return " +";
    else if (what == adobe::remove_k)
        return " -";
    else if (what == adobe::enable_k)
        return " E";
    else if (what == adobe::disable_k)
        return " D";
    else if (what == (adobe::add_k | adobe::enable_k))
        return "+E";
    else if (what == (adobe::add_k | adobe::disable_k))
        return "+D";

    return " ?";
}

/**************************************************************************************************/

void binary_sentence_notifier(const adobe::poly_sentence_t&      verb,
                              const adobe::poly_subject_t&       subject,
                              const adobe::poly_direct_object_t& object,
                              adobe::event_t                     what)
{
    std::cout << "   " << event_to_char(what)
              << "; subject: "
              << reinterpret_cast<const void*>(&subject);

    //if (subject.type_m != adobe::name_t())
    //    std::cout << '(' << subject.type_m << ')';

    std::cout << "; dirobj: "
              << reinterpret_cast<const void*>(&object);

    //if (object.type_m != adobe::name_t())
    //    std::cout << '(' << object.type_m << ')';

    std::cout << "; verb: "
              << reinterpret_cast<const void*>(&verb)
              << std::endl;
}

/**************************************************************************************************/

void unary_sentence_notifier(const adobe::poly_sentence_t& verb,
                             const adobe::poly_subject_t&  subject,
                             adobe::event_t                what)
{
    std::cout << "   " << event_to_char(what)
              << "; subject: "
              << reinterpret_cast<const void*>(&subject);

    //if (subject.type_m != adobe::name_t())
    //    std::cout << '(' << subject.type_m << ')';

    std::cout << "; verb: "
              << reinterpret_cast<const void*>(&verb)
              << std::endl;
}

/**************************************************************************************************/

const adobe::event_t nonexistent(adobe::remove_k);
const adobe::event_t exists_and_enabled(adobe::add_k | adobe::enable_k);
const adobe::event_t exists_and_disabled(adobe::add_k | adobe::disable_k);

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE( simple_command_system )
{
    bool                    verbose(false);
    adobe::command_system_t system;

    /* Construct Sentence(s) */

    adobe::command_system_t::sentence_token_t diff_token =
        system.insert(adobe::make_expression_sentence(adobe::static_name_t("diff_user_mask"),
                                                      "receives(@raster_mask) && require(@has_user_mask)",
                                                      "emits(@raster_mask)"));
    adobe::command_system_t::sentence_token_t del_token =
        system.insert(adobe::make_expression_sentence(adobe::static_name_t("delete_user_mask"),
                                                      "require(@has_user_mask)"));

    /* Set up Notifiers */

    if (verbose)
    {
        system.monitor(diff_token, adobe::binary_monitor_proc_t(binary_sentence_notifier));
        system.monitor(del_token, adobe::unary_monitor_proc_t(unary_sentence_notifier));
    }

    /* Construct Subject(s) */

    adobe::command_system_t::subject_token_t subj1 =
        system.insert(system.no_parent_subject(), adobe::poly_subject_t(dictionary_sentence_thing_t(true, adobe::static_name_t("foo"))));
    adobe::command_system_t::subject_token_t subj2 =
        system.insert(system.no_parent_subject(), adobe::poly_subject_t(dictionary_sentence_thing_t(false, adobe::static_name_t("bar"))));

    /* Construct Direct Object(s) */

    adobe::command_system_t::direct_object_token_t dirobj1 =
        system.insert(system.no_parent_direct_object(), adobe::poly_direct_object_t(dictionary_sentence_thing_t(true, adobe::static_name_t("baz"))));

    /* Test engine */
	BOOST_CHECK(system.inspect(del_token, subj1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, subj1, dirobj1) == exists_and_enabled);

    if (verbose) std::cout << "delete_user_mask sentence: disable" << std::endl;
    system.enable(del_token, false);
	BOOST_CHECK(del_token->second == false);

    if (verbose) std::cout << "delete_user_mask sentence: enable" << std::endl;
    system.enable(del_token, true);
	BOOST_CHECK(del_token->second == true);

    if (verbose) std::cout << "subj1: disable" << std::endl;
    system.enable(subj1, false);
	BOOST_CHECK(subj1->second == false);
	BOOST_CHECK(system.inspect(del_token, subj1) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, subj1, dirobj1) == exists_and_disabled);

    if (verbose) std::cout << "delete_user_mask sentence: disable" << std::endl;
    system.enable(del_token, false);
	BOOST_CHECK(del_token->second == false);

    if (verbose) std::cout << "subj1: enable and delete_user_mask sentence: enable" << std::endl;
    system.enable(subj1, true);
    system.enable(del_token, true);
	BOOST_CHECK(system.inspect(del_token, subj1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, subj1, dirobj1) == exists_and_enabled);

    if (verbose) std::cout << "delete_user_mask sentence: disable" << std::endl;
    system.enable(del_token, false);
	BOOST_CHECK(system.inspect(del_token, subj1) == exists_and_disabled);

    if (verbose) std::cout << "subj2: has_user_mask = true" << std::endl;
    subj2->first.cast<dictionary_sentence_thing_t>().has_m = true;
    system.update(subj2);
	BOOST_CHECK(system.inspect(del_token, subj2) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, subj2, dirobj1) == exists_and_enabled);

    if (verbose) std::cout << "delete_user_mask sentence: enable" << std::endl;
    system.enable(del_token, true);
	BOOST_CHECK(system.inspect(del_token, subj1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(del_token, subj2) == exists_and_enabled);

    if (verbose) std::cout << "subj2: has_user_mask = false" << std::endl;
    subj2->first.cast<dictionary_sentence_thing_t>().has_m = false;
    system.update(subj2);
	BOOST_CHECK(system.inspect(del_token, subj2) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, subj2, dirobj1) == nonexistent);

    if (verbose) std::cout << "delete_user_mask sentence: erase" << std::endl;
    system.erase(del_token);

    if (verbose) std::cout << "End test: 'simple_command_system'" << std::endl;
}

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

class application_t : boost::equality_comparable<application_t>, boost::noncopyable
{
public:
    typedef const application_t* subject_type;

    subject_type project() const { return this; }
};

bool operator==(const application_t& x, const application_t& y)
{ return &x == &y; }

std::ostream& operator<<(std::ostream& s, const application_t&)
{ return s; }

/**************************************************************************************************/

class document_t : boost::equality_comparable<document_t>, boost::noncopyable
{
public:
    typedef const document_t* subject_type;

    subject_type project() const { return this; }
};

bool operator==(const document_t& x, const document_t& y)
{ return &x == &y; }

std::ostream& operator<<(std::ostream& s, const document_t&)
{ return s; }

/**************************************************************************************************/

class layer_t : boost::equality_comparable<layer_t>, boost::noncopyable
{
public:
    typedef const layer_t* subject_type;
    typedef const layer_t* direct_object_type;

    subject_type project() const { return this; }
    //direct_object_type project() const { return this; }

    explicit layer_t(bool has = false) :
        has_layer_mask_m(has)
    { }

    bool has_layer_mask_m;
};

bool operator==(const layer_t& x, const layer_t& y)
{ return &x == &y; }

std::ostream& operator<<(std::ostream& s, const layer_t&)
{ return s; }

/**************************************************************************************************/

struct verb_quit_t
{
public:
    typedef const application_t* subject_type;

    bool test_subject(const subject_type& /*subject*/)
    {
        return true;
    }
};

/**************************************************************************************************/

struct verb_delete_user_mask_t
{
public:
    typedef const layer_t* subject_type;

    bool test_subject(const subject_type& subject)
    {
        return subject->has_layer_mask_m;
    }
};

/**************************************************************************************************/

struct verb_diff_user_mask_t
{
public:
    typedef const layer_t* subject_type;
    typedef const layer_t* direct_object_type;

    bool test_subject(const subject_type& subject)
    {
        return subject->has_layer_mask_m;
    }

    bool test_direct_object(const subject_type& subject, const direct_object_type& dirobj)
    {
        return subject != dirobj && dirobj->has_layer_mask_m;
    }
};

/**************************************************************************************************/

struct verb_copy_user_mask_t
{
public:
    typedef const layer_t* subject_type;
    typedef const layer_t* direct_object_type;

    bool test_subject(const subject_type& subject)
    {
        return subject->has_layer_mask_m == false;
    }

    bool test_direct_object(const subject_type& /*subject*/, const direct_object_type& dirobj)
    {
        /*
            'subject != dirobj' is not necessary; one has a layer mask while the
            other would not, so the subject and direct object cannot be the same.
        */
        return dirobj->has_layer_mask_m;
    }
};

/**************************************************************************************************/

struct monitor_quit_t
{
    typedef verb_quit_t sentence_type;

    void operator ()(const sentence_type::subject_type& subject, adobe::event_t what)
    {
        std::cout << "   " << event_to_char(what)
                  << "; verb: quit"
                  << "; subject: " << reinterpret_cast<const void*>(subject)
                  << std::endl;
    }
};

/**************************************************************************************************/

struct monitor_delete_mask_t
{
    typedef verb_delete_user_mask_t sentence_type;

    void operator ()(const sentence_type::subject_type& subject, adobe::event_t what)
    {
        std::cout << "   " << event_to_char(what)
                  << "; verb: delete_user_mask"
                  << "; subject: " << reinterpret_cast<const void*>(subject)
                  << std::endl;
    }
};

/**************************************************************************************************/

struct monitor_diff_mask_t
{
    typedef verb_diff_user_mask_t sentence_type;

    void operator ()(const sentence_type::subject_type&       subject,
                     const sentence_type::direct_object_type& dirobj,
                     adobe::event_t                           what)
    {
        std::cout << "   " << event_to_char(what)
                  << "; verb: diff_user_mask"
                  << "; subject: " << reinterpret_cast<const void*>(subject)
                  << "; dirobj: " << reinterpret_cast<const void*>(dirobj)
                  << std::endl;
    }
};

/**************************************************************************************************/

struct monitor_copy_mask_t
{
    typedef verb_copy_user_mask_t sentence_type;

    void operator ()(const sentence_type::subject_type&       subject,
                     const sentence_type::direct_object_type& dirobj,
                     adobe::event_t                           what)
    {
        std::cout << "   " << event_to_char(what)
                  << "; verb: copy_user_mask"
                  << "; subject: " << reinterpret_cast<const void*>(subject)
                  << "; dirobj: " << reinterpret_cast<const void*>(dirobj)
                  << std::endl;
    }
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

ADOBE_NAME_TYPE_0("application_t", application_t)
ADOBE_NAME_TYPE_0("document_t", document_t)
ADOBE_NAME_TYPE_0("layer_t", layer_t)

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE( complex_command_system )
{
    typedef adobe::command_system_t::sentence_token_t sentence_token_t;
    typedef adobe::command_system_t::subject_token_t  subject_token_t;
    typedef adobe::command_system_t::noun_token_t     noun_token_t;

    bool                    verbose(true);
    adobe::command_system_t system;
    application_t           application;
    document_t              document;
    layer_t                 layer1(true);
    layer_t                 layer2(false);
    layer_t                 layer3(true);

    /* Construct Sentence(s) */

    sentence_token_t quit_token(system.insert_sentence(verb_quit_t()));
    sentence_token_t diff_token(system.insert_binary_sentence(verb_diff_user_mask_t()));
    sentence_token_t copy_token(system.insert_binary_sentence(verb_copy_user_mask_t()));
    sentence_token_t del_token(system.insert_sentence(verb_delete_user_mask_t()));

    /* Set up Notifiers */

    if (verbose)
    {
        system.monitor(quit_token, adobe::make_unary_notifier(monitor_quit_t()));
        system.monitor(diff_token, adobe::make_binary_notifier(monitor_diff_mask_t()));
        system.monitor(copy_token, adobe::make_binary_notifier(monitor_copy_mask_t()));
        system.monitor(del_token, adobe::make_unary_notifier(monitor_delete_mask_t()));
    }

    /* Construct Subject(s) */

    if (verbose) std::cout << "application: insert subject" << std::endl;

    subject_token_t app(system.insert_subject(system.no_parent_subject(), boost::ref(application)));

	BOOST_CHECK(system.inspect(quit_token, app) == exists_and_enabled);

    if (verbose) std::cout << "document: insert subject" << std::endl;

    subject_token_t doc1(system.insert_subject(app, boost::ref(document)));

	BOOST_CHECK(system.inspect(quit_token, doc1) == nonexistent);

    /* Construct Noun(s) */

    if (verbose) std::cout << "layer1: insert noun" << std::endl;

    noun_token_t doc1layer1(system.insert_noun(doc1, boost::ref(layer1)));

	BOOST_CHECK(system.inspect(del_token, doc1layer1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer1, doc1layer1) == nonexistent);

    if (verbose) std::cout << "layer2: insert noun" << std::endl;

    noun_token_t doc1layer2(system.insert_noun(doc1, boost::ref(layer2)));

	BOOST_CHECK(system.inspect(del_token, doc1layer2) == nonexistent);
	BOOST_CHECK(system.inspect(copy_token, doc1layer2, doc1layer1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer1, doc1layer2) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer1) == nonexistent);

    if (verbose) std::cout << "layer3: insert noun" << std::endl;

    noun_token_t doc1layer3(system.insert_noun(doc1, boost::ref(layer3)));

	BOOST_CHECK(system.inspect(del_token, doc1layer3) == exists_and_enabled);
	BOOST_CHECK(system.inspect(copy_token, doc1layer2, doc1layer3) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer1, doc1layer3) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer2) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer3) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer2) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer3) == nonexistent);

    /* Do Stuff */

    if (verbose) std::cout << "layer2: set has layer mask = true" << std::endl;

    layer2.has_layer_mask_m = true;
    system.update(doc1layer2);

	BOOST_CHECK(system.inspect(del_token, doc1layer2) == exists_and_enabled);
	BOOST_CHECK(system.inspect(copy_token, doc1layer2, doc1layer1) == nonexistent);
	BOOST_CHECK(system.inspect(copy_token, doc1layer2, doc1layer3) == nonexistent);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer1) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer3) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer2) == exists_and_enabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer1, doc1layer2) == exists_and_enabled);

    if (verbose) std::cout << "layer3: disable" << std::endl;

    system.enable(doc1layer3, false);

	BOOST_CHECK(system.inspect(del_token, doc1layer3) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer1) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer3, doc1layer2) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer1, doc1layer3) == exists_and_disabled);
	BOOST_CHECK(system.inspect(diff_token, doc1layer2, doc1layer3) == exists_and_disabled);

    if (verbose) std::cout << "End test: 'complex_command_system'" << std::endl;
}

/**************************************************************************************************/
