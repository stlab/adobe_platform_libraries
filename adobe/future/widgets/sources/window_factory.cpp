/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <adobe/future/platform_primitives.hpp>

// window.hpp needs to come before widget_factory to hook the overrides
#include <adobe/future/widgets/headers/platform_window.hpp>

#include <adobe/future/widgets/headers/window_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory.hpp>
#include <adobe/future/widgets/headers/widget_factory_registry.hpp>
/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

void create_widget(const dictionary_t& parameters,
                   size_enum_t         size,
                   window_t*&          window)
{
    std:: string name;
    bool         grow(false);
    bool         metal(false);

    get_value(parameters, key_name, name);
    get_value(parameters, key_grow, grow);
    get_value(parameters, key_metal, metal);

    window_attributes_t attributes(window_attributes_standard_handler_s);

    if (grow)
        attributes |= window_attributes_resizeable_s |
                      window_attributes_live_resizeable_s;

    if (metal)
        attributes |= window_attributes_metal_s;

    window = new window_t(name,
                        window_style_moveable_modal_s,
                        attributes,
                        window_modality_none_s,
                        implementation::size_to_theme(size));
}

/****************************************************************************************************/

widget_node_t make_window(const dictionary_t&     parameters,
                          const widget_node_t&    parent,
                          const factory_token_t&  token,
                          const widget_factory_t& factory)
{
    bool is_container(factory.is_container(static_name_t("dialog")));
    const layout_attributes_t& layout_attributes(
        factory.layout_attributes(static_name_t("dialog")));
    size_enum_t size(parameters.count(key_size) ?
                     implementation::enumerate_size(get_value(parameters, key_size).cast<name_t>()) :
                     parent.size_m);

    window_t* widget(NULL);
    create_widget(parameters, size, widget);
    token.client_holder_m.assemblage_m.cleanup(boost::bind(delete_ptr(), widget));

    //
    // Call display_insertion to embed the new widget within the view heirarchy
    //
    platform_display_type display_token(insert(get_main_display(), 
                                               parent.display_token_m, *widget));

    token.client_holder_m.root_display_m = display_token;

    // This is a top-level container, so we hook it up to the show_window_signal
    // (so that it gets shown when all of the children have been made and inserted).
    //
    token.client_holder_m.assemblage_m.cleanup(boost::bind(&boost::signals::connection::disconnect,
        token.client_holder_m.show_window_m.connect(
          boost::bind(&window_t::set_visible, boost::ref(*widget), true))));

    //
    // As per SF.net bug 1428833, we want to attach the poly_placeable_t
    // to Eve before we attach the controller and view to the model
    //

    eve_t::iterator eve_token =
        attach_placeable<poly_placeable_t>(
            parent.eve_token_m, *widget, parameters, token, is_container, layout_attributes);

    widget->monitor_resize(boost::bind(&eve_t::adjust, boost::ref(token.client_holder_m.eve_m), 
        eve_t::evaluate_nested, _1, _2));

    // set up key handler code. We do this all the time because we want the button to be updated
    // when modifier keys are pressed during execution of the dialog.

    keyboard_t::iterator keyboard_token(keyboard_t::get().insert(keyboard_t::iterator(), poly_key_handler_t(boost::ref(*widget))));

    token.client_holder_m.assemblage_m.cleanup(
        boost::bind(&keyboard_t::erase, boost::ref(keyboard_t::get()), keyboard_token));

    //
    // Return the widget_node_t that comprises the tokens created for this widget by the various components
    //
    return widget_node_t(size, eve_token, display_token, keyboard_token);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
