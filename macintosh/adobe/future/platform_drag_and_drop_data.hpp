/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_CARBON_DRAG_AND_DROP_PLATFORM_DATA_HPP
#define ADOBE_CARBON_DRAG_AND_DROP_PLATFORM_DATA_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/future/drag_and_drop_fwd.hpp>
#include <adobe/future/macintosh_events.hpp>

#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/next_prior.hpp>

#include <cassert>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

typedef std::pair< ::DragItemRef, ::UInt16 >                 drag_flavor_descriptor_t;
typedef std::pair<drag_flavor_descriptor_t, boost::uint32_t> drag_flavor_uber_descriptor_t;

template <typename InputIterator>
drag_flavor_uber_descriptor_t find_drag_flavor(::DragRef     the_drag,
                                               InputIterator first,
                                               InputIterator last)
{
    ::UInt16                 count(0);
    drag_flavor_descriptor_t result(0, 0);

    ::CountDragItems(the_drag, &count);

    for (::UInt16 cur_item_index(1); cur_item_index <= count; ++cur_item_index)
    {
        ::DragItemRef drag_item(0);

        if (::GetDragItemReferenceNumber(the_drag, cur_item_index, &drag_item) != noErr)
            continue;

        ::UInt16 flavor_count(0);

        if (::CountDragItemFlavors(the_drag, drag_item, &flavor_count) != noErr)
            continue;

        for (::UInt16 cur_flavor(1); cur_flavor <= flavor_count; ++cur_flavor)
        {
            ::FlavorType flavor_type(0);

            if (::GetFlavorType(the_drag, drag_item, cur_flavor, &flavor_type) == noErr)
            {
                InputIterator found(std::find(first, last, flavor_type));

                if (found != last)
                {
                    result.first = drag_item;
                    result.second = cur_flavor;

                    return drag_flavor_uber_descriptor_t(result, *found);
                }
            }
        }
    }

    return drag_flavor_uber_descriptor_t(result, flavor_invalid);
}

/**************************************************************************************************/

template <typename InputIterator>
drag_flavor_uber_descriptor_t find_drag_flavor(::EventRef    event,
                                               InputIterator first,
                                               InputIterator last)
{
    ::DragRef the_drag(0);

    adobe::get_event_parameter<kEventParamDragRef>(event, the_drag);

    return find_drag_flavor(the_drag, first, last);
}

/**************************************************************************************************/

inline drag_flavor_uber_descriptor_t find_drag_flavor(::DragRef the_drag, boost::uint32_t flavor)
{
    boost::uint32_t* first(&flavor);

    return find_drag_flavor(the_drag, first, boost::next(first));
}

/**************************************************************************************************/

inline drag_flavor_uber_descriptor_t find_drag_flavor(::EventRef event, boost::uint32_t flavor)
{
    boost::uint32_t* first(&flavor);

    return find_drag_flavor(event, first, boost::next(first));
}

/**************************************************************************************************/

any_regular_t flavor_extract_character_sequence(const dictionary_t& drag_parameters);

/**************************************************************************************************/

/// \todo REVISIT (fbrereto): Move this to a more generic location, and have it return a
///                           boost::filesystem::path instead of a string (or at least a type
///                           that is more path-context-specific than "string").
std::string file_url_to_path(const std::string& src);

/**************************************************************************************************/

template <typename T>
struct converter_echo
{
    typedef T source_type;
    typedef T dest_type;

    inline T convert(const T& x) const
    { return x; }
};

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

extern aggregate_name_t key_drag_ref;
extern aggregate_name_t key_drag_item;
extern aggregate_name_t key_flavor;

/**************************************************************************************************/

template <typename SourceType, typename TargetType>
class drag_and_drop_handler_platform_data
{
public:
    typedef SourceType                                 source_type;
    typedef TargetType                                 target_type;
    typedef boost::function<void (const target_type&)> client_callback_proc_t;

    drag_and_drop_handler_platform_data() :
        chosen_flavor_m(flavor_invalid)
    {
        event_handler_m.monitor_proc_m =
            boost::bind(&drag_and_drop_handler_platform_data<source_type, target_type>::handle_event,
                        boost::ref(*this), _1, _2);
    }

    template <typename InputIterator>
    drag_and_drop_handler_platform_data(InputIterator first, InputIterator last) :
        flavor_set_m(first, last),
        chosen_flavor_m(flavor_invalid)
    {
        event_handler_m.monitor_proc_m =
            boost::bind(&drag_and_drop_handler_platform_data<source_type, target_type>::handle_event,
                        boost::ref(*this), _1, _2);
    }

    void init()
    {
        event_handler_m.insert(kEventClassControl, kEventControlDragEnter);
        event_handler_m.insert(kEventClassControl, kEventControlDragWithin);
        event_handler_m.insert(kEventClassControl, kEventControlDragLeave);
        event_handler_m.insert(kEventClassControl, kEventControlDragReceive);

        // REVISIT (fbrereto) : adobe::once this!
        // set up platform-specific flavor converters
        if (is_dnd_converter_registered<std::string>(flavor_file_url) == false)
        {
            register_dnd_extractor<std::string>(flavor_file_url,
                                                &implementation::flavor_extract_character_sequence);
            register_dnd_extractor<std::string>(flavor_text,
                                                &implementation::flavor_extract_character_sequence);

            register_dnd_converter<std::string>(flavor_file_url,
                                                make_function_as_poly_drag_and_drop_converter
                                                    (&implementation::file_url_to_path));
            register_dnd_converter<std::string>(flavor_text,
                                                poly_drag_and_drop_converter_t
                                                    (implementation::converter_echo<std::string>()));
        }
    }

    template <typename Client>
    void attach(const Client& control, const client_callback_proc_t& proc)
    {
        set_callback(proc);

        install(control);
    }

    void detach()
    {
        event_handler_m.uninstall();
    }

    void insert_flavor(boost::uint32_t flavor)
    {
        if (adobe::find(flavor_set_m, flavor) == flavor_set_m.end())
            flavor_set_m.push_back(flavor);
    }

    void erase_flavor(boost::uint32_t flavor)
    {
        flavor_set_m.erase(adobe::find(flavor_set_m, flavor));
    }

    void set_callback(const client_callback_proc_t& proc)
    {
        callback_m = proc;
    }

    void install(::HIViewRef control)
    {
        event_handler_m.install(control);

        ::SetControlDragTrackingEnabled(control, true);
    }

private:
    ::OSStatus handle_event(::EventHandlerCallRef next, ::EventRef event);

    client_callback_proc_t       callback_m;
    std::vector<boost::uint32_t> flavor_set_m;
    boost::uint32_t              chosen_flavor_m;
    event_handler_t              event_handler_m;
};

/**************************************************************************************************/

template <typename SourceType, typename TargetType>
::OSStatus
drag_and_drop_handler_platform_data<SourceType, TargetType>::handle_event(::EventHandlerCallRef /*next*/,
                                                                          ::EventRef            event)
{
   ::UInt32 event_class(::GetEventClass(event));
   ::UInt32 event_kind(::GetEventKind(event));

    assert(event_class == kEventClassControl);

    std::vector<boost::uint32_t> flavor_set(flavor_set_m.empty() ? registered_flavor_set() :
                                                                   flavor_set_m);

    if (flavor_set.empty())
        return eventNotHandledErr; // drag and drop is totally out of comission

    if (event_kind == kEventControlDragEnter)
    {
        if (callback_m.empty())
            return eventNotHandledErr;
    
        implementation::drag_flavor_uber_descriptor_t flavor =
            implementation::find_drag_flavor(event, flavor_set.begin(), flavor_set.end());

        if (flavor.second != flavor_invalid && callback_m.empty() == false)
        {
            adobe::set_event_parameter<kEventParamControlWouldAcceptDrop>(event, true);

            chosen_flavor_m = flavor.second;
        }
    }
    else if (event_kind == kEventControlDragWithin)
    {
        // REVISIT (fbrereto) : Fix me up
        ::SetThemeCursor(kThemeCopyArrowCursor);
    }
    else if (event_kind == kEventControlDragLeave)
    {
        // REVISIT (fbrereto) : Fix me up
        ::SetThemeCursor(kThemeArrowCursor);
    }
    else if (event_kind == kEventControlDragReceive)
    {
        ::DragRef the_drag(0);

        adobe::get_event_parameter<kEventParamDragRef>(event, the_drag);

        implementation::drag_flavor_uber_descriptor_t flavor =
            implementation::find_drag_flavor(the_drag, chosen_flavor_m);

        assert(flavor.second == chosen_flavor_m);
        assert(callback_m.empty() == false);

        dictionary_t drag_parameters;

        drag_parameters.insert(std::make_pair(key_drag_ref, the_drag));
        drag_parameters.insert(std::make_pair(key_drag_item, flavor.first.first));
        drag_parameters.insert(std::make_pair(key_flavor, chosen_flavor_m));

        source_type flavor_data(invoke_dnd_extractor<source_type>(chosen_flavor_m,
                                                                  drag_parameters));

        target_type converted_result =
            invoke_dnd_converter<target_type>(chosen_flavor_m, flavor_data);

        callback_m(converted_result);

        chosen_flavor_m = flavor_invalid;
    }
    else
    {
        throw std::runtime_error("Bad drag and drop event.");
    }

    return noErr;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_CARBON_DRAG_AND_DROP_HPP
#endif

/**************************************************************************************************/
