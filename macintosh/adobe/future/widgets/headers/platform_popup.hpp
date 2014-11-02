/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/****************************************************************************************************/

#ifndef ADOBE_WIDGET_POPUP_HPP
#define ADOBE_WIDGET_POPUP_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/extents.hpp>
#include <adobe/future/debounce.hpp>
#include <adobe/future/macintosh_events.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/popup_common_fwd.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/memory.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
class sheet_t;
class assemblage_t;

struct popup_t;

/****************************************************************************************************/

struct popup_value_handler_t
{
    explicit popup_value_handler_t(popup_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassControl, kEventControlValueFieldChanged);

        handler_m.monitor_proc_m = boost::bind(&popup_value_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(    ::EventHandlerCallRef   next,
                                ::EventRef              event);

    event_handler_t            handler_m;
    popup_setter_type          setter_m;
    popup_extended_setter_type extended_setter_m;
    popup_t&                   widget_m;
};

/****************************************************************************************************/

struct popup_key_handler_t
{
    popup_key_handler_t()
    {
        handler_m.insert(kEventClassTextInput, kEventTextInputUnicodeForKeyEvent);

        handler_m.monitor_proc_m = boost::bind(&popup_key_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t handler_m;
    popup_t*        popup_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Popup widget

    \model_of
        - \ref concept_placeable

    The semantics of a popup are that of a selector between enumerated values. One would use it
    when you have a specific set of options from which the user may choose one. At this time the
    selection of multiple options in the popup at once is not supported.
*/
struct popup_t
{
    /// model type for this widget
    typedef any_regular_t model_type;

    /// controller callback proc type for this widget
    typedef popup_setter_type          setter_type;
    typedef popup_extended_setter_type extended_setter_type;

#ifndef ADOBE_NO_DOCUMENTATION
    typedef std::pair<std::string, any_regular_t> menu_item_t;
    typedef std::vector<menu_item_t> menu_item_set_t;
#endif

    /*!
        \param name             Name of the widget
        \param alt_text         Additional help text for the widget when the user pauses over it
        \param custom_item_name Name to be used for when no item in the cell matches a popup item
        \param first            The first menu item in the list
        \param last             One-past-the-back of the menu item list
        \param theme            Theme for the widget
    */
    popup_t(const std::string& name,
            const std::string& alt_text,
            const std::string& custom_item_name,
            const menu_item_t* first,
            const menu_item_t* last,
            theme_t            theme);

    /*!
        \param first       The first menu item in the list
        \param last        One-past-the-back of the menu item list

        Resets the popup with a new list of menu items.
    */
    void reset_menu_item_set(const menu_item_t* first, const menu_item_t* last);

    /*!
        Resets the popup with a new list of menu items.
    */
    inline void reset_menu_item_set(const menu_item_set_t& menu_item_set)
    {
        const menu_item_t* first(menu_item_set.empty() ? 0 : &menu_item_set[0]);
        std::size_t        n(menu_item_set.size());

        reset_menu_item_set(first, first + n);
    }

    /*!
        \param text the text to match to the popup menu entry

        Tries to set the currently selected option in the popup by matching a string
    */
    void select_with_text(const std::string& text);

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

    /*!
        @name View Concept Operations
        @{

        See the \ref concept_view concept and \ref view.hpp for more information.
    */
    void display(const any_regular_t& value);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const setter_type& proc);

    void enable(bool make_enabled);
    ///@}

    void monitor_extended(const extended_setter_type& extended_proc);

#ifndef ADOBE_NO_DOCUMENTATION
    ::ControlRef               control_m;
    theme_t                    theme_m;
    mutable metric_extractor_t metrics_m;
    std::string                name_m;
    std::string                alt_text_m;
    bool                       static_disabled_m;
    bool                       using_label_m;
    long                       label_width_m;
    menu_item_set_t            menu_items_m;
    popup_value_handler_t      value_handler_m;
    popup_key_handler_t        key_handler_m;
    bool                       type_2_debounce_m;
    any_regular_t              last_m;
    bool                       custom_m;
    std::string                custom_item_name_m;

private:
    void display_custom();
#endif
};

/****************************************************************************************************/

namespace view_implementation {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
inline void set_value_from_model(popup_t& value, const any_regular_t& new_value)
{ value.display(new_value); }
#endif
/****************************************************************************************************/

} // namespace view_implementation

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
inline bool operator== (const popup_t& x, const popup_t& y)
{ return &x == &y; }
#endif
/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
