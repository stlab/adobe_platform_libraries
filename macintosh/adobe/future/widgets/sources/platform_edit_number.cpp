/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <adobe/future/widgets/headers/platform_edit_number.hpp>
#include <adobe/future/widgets/headers/edit_number.hpp>

#include <adobe/future/cursor.hpp>
#include <adobe/future/resources.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void edit_number_platform_data_t::initialize()
{
    wheel_handler_m.callback_m = boost::bind(&edit_number_platform_data_t::wheel,
                                             boost::ref(*this), _1, _2);
    wheel_handler_m.handler_m.install(control_m->edit_text().control_m);

    if (control_m->edit_text().using_label_m)
        control_m->edit_text().get_label().subevent_m.callback_m =
            boost::bind(&edit_number_platform_data_t::label_subevent, boost::ref(*this), _1);
}

/****************************************************************************************************/

void edit_number_platform_data_t::label_subevent(label_subevent_t type)
{
    static adobe_cursor_t cursor(edit_number_t::scrubby_cursor());

    if (type == subevent_mouse_down_s)
    {
        ::Point last_point = { 0 };

        while (true)
        {
            ::Point               cur_point = { 0 };
            ::MouseTrackingResult track_result(0);
            ::UInt32              os_modifiers(0);

            ::TrackMouseLocationWithOptions(0, /*inPort*/
                                            0, /*inOptions*/
                                            .1, /*inTimeout (in seconds)*/
                                            &cur_point,
                                            &os_modifiers,
                                            &track_result);

            modifiers_t modifiers(implementation::convert_modifiers(os_modifiers));

            if (track_result == kMouseTrackingMouseUp)
                break;

            if (last_point.v != 0)
            {
                long delta(last_point.v - cur_point.v);

                if (modifiers & modifiers_any_shift_s)
                    delta *= 10;

                control_m->increment_n(delta);
            }

            last_point = cur_point;
        }
    }
    else if (cursor != 0)
    {
        if (type == subevent_mouse_in_s)
        {
            push_cursor(cursor);
        }
        else if (type == subevent_mouse_out_s)
        {
            pop_cursor();
        }
    }
}

/****************************************************************************************************/

void edit_number_platform_data_t::scrubby(double new_value)
{
    typedef edit_number_t::model_type model_type;
    typedef unit_t     unit_type;

    unit_type   unit(control_m->unit_set_m[control_m->unit_index_m]);
    model_type  base_value(new_value);
    model_type  scale_value(to_scaled_value(base_value, unit));

    std::string new_string_value(control_m->number_formatter_m.format(scale_value));

    control_m->edit_text().display(new_string_value);
    control_m->monitor_text(new_string_value);
}

/****************************************************************************************************/

void edit_number_platform_data_t::wheel(long delta, bool extra)
{
    typedef edit_number_t::model_type model_type;
    typedef unit_t     unit_type;

    std::size_t base_index(control_m->current_base_unit_index());
    unit_type   unit(control_m->unit_set_m[control_m->unit_index_m]);
    double      increment(unit.increment_m);
    model_type  base_value(control_m->debounce_set_m[base_index]);
    model_type  scale_value(to_scaled_value(base_value, unit));

    if (extra)
        delta *= 10;

    scale_value += delta * increment;

    std::string new_value(control_m->number_formatter_m.format(scale_value));

    control_m->monitor_text(new_value, false);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
