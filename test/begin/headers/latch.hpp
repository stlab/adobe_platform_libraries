/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_LATCH_HPP
#define ADOBE_LATCH_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/value.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

template <typename ValueType>
struct latch_t
{
    typedef ValueType                                       value_type;
    typedef boost::function<void (const value_type&, bool)> trigger_t;
    typedef boost::function<void (bool)>                    valid_t;

    template <typename TriggerFunction, typename ValidFunction>
    latch_t(const TriggerFunction& trigger, const ValidFunction& valid) :
        trigger_m(trigger), valid_m(valid), force_m(true)
    { }

    latch_t(const latch_t&) : force_m(true)
    { }

    latch_t& operator = (const latch_t&)
    {
        // Clear connections on copy
        force_m = false;
        trigger_m = trigger_t();
        valid_m = valid_t();

        return *this;
    }

    template <typename TriggerFunction, typename ValidFunction>
    void attach(const TriggerFunction& trigger, const ValidFunction& valid)
    {
        trigger_m = trigger;
        valid_m = valid;
    }

    void set_value(const value_t& new_value, bool is_valid)
    {
        if (is_valid != is_valid_m || force_m) valid_m(is_valid);

        value_m = new_value;
        is_valid_m = is_valid;
        force_m = false;
    }

    void trigger() const
    {
        /* REVISIT (sparent) : should this conditional simply be a pre-condition */
        if (!force_m) trigger_m(value_m, is_valid_m);
    }

private:

    value_type      value_m;
    bool            is_valid_m;
    bool            force_m;

    trigger_t       trigger_m;
    valid_t         valid_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif // ADOBE_LATCH_HPP

/****************************************************************************************************/
