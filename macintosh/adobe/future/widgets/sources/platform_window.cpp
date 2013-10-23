/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#define ADOBE_DLL_SAFE 0

#include <adobe/future/widgets/headers/platform_window.hpp>

#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(window_t, "<xstr id='metric:window'/>");

/**************************************************************************************************/

::OSStatus window_resize_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                 ::EventRef            /*event*/)
{
    if (window_m.debounce_m)
        return eventNotHandledErr;

    if (!callback_m)
        return noErr;

    window_m.debounce_m = true;

    try
    {
        Rect bounds = { 0 };

        ::GetWindowBounds(window_m.window_m, kWindowContentRgn, &bounds);

        point_2d_t new_dimensions(bounds.right - bounds.left, bounds.bottom - bounds.top);

        if (new_dimensions != window_m.dimensions_m)
        {
            window_m.dimensions_m = new_dimensions;

            callback_m(new_dimensions.x_m, new_dimensions.y_m);
        }
    }
    catch (...)
    {
        window_m.debounce_m = false;
        throw;
    }

    window_m.debounce_m = false;
    
    return noErr;
}

/**************************************************************************************************/

::OSStatus window_close_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                                ::EventRef            /*event*/)
{
    // for some reason this isn't getting hit by Carbon...

    ::AudioServicesPlayAlertSound(kUserPreferredAlert);

    return noErr;
}

/**************************************************************************************************/

window_t::window_t(const std::string&  name,
                   window_style_t      style,
                   window_attributes_t attributes,
                   window_modality_t   modality,
                   theme_t             theme) :
    window_m(0),
    root_control_m(0),
    name_m(name),
    style_m(style),
    attributes_m(attributes),
    modality_m(modality),
    theme_m(theme),
    debounce_m(false),
    resize_handler_m(*this),
    close_handler_m(*this),
    placed_once_m(false)
{ }

/**************************************************************************************************/

window_t::~window_t()
{
    if (window_m)
        ::DisposeWindow(window_m);
}

/**************************************************************************************************/

void window_t::measure(extents_t& result)
{
    assert(window_m);

    measure_label_text(name_m, theme_m, result);

    implementation::set_metric_extractor(*this);

    result.width() += metrics_m(implementation::k_metric_spacing);

    // This is intended to make room for the grow icon on the mac. We alter the frame
    // because the grow icon should not be considered part of the internal real estate
    // of the window. (or maybe it should, in the case when scroll bars are present and
    // flush with the grow icon?)

    if (attributes_m & window_attributes_resizeable_s)
    {
        /* REVISIT FIXED VALUE */
        result.horizontal().frame_m.second = 15; 
        result.vertical().frame_m.second = 15;
    }

    result = implementation::apply_fudges(*this, result);
}

/**************************************************************************************************/

void window_t::place(const place_data_t& place_data)
{
    assert(window_m);

    if (placed_once_m)
    {
        set_size(point_2d_t(width(place_data), height(place_data)));
    }
    else
    {
        if (debounce_m)
            return;
    
        debounce_m = true;
        
        placed_once_m = true;

        place_data_t bounds(place_data);

        bounds.vertical().position_m = 0;
        bounds.horizontal().position_m = 0;

        ::Rect win_rect =
        {
            static_cast<short>(top(bounds)),
            static_cast<short>(left(bounds)),
            static_cast<short>(bottom(bounds)),
            static_cast<short>(right(bounds))
        };

        dimensions_m.x_m = width(bounds);
        dimensions_m.y_m = height(bounds);

        ::HISize min_size = { static_cast<CGFloat>(dimensions_m.x_m), static_cast<CGFloat>(dimensions_m.y_m) };

        ADOBE_REQUIRE_STATUS(::SetWindowResizeLimits(window_m, &min_size, NULL));

        ADOBE_REQUIRE_STATUS(::SetWindowBounds(window_m, kWindowContentRgn, &win_rect));

        reposition(window_reposition_center_s);

        debounce_m = false;
    }
}

/**************************************************************************************************/

void window_t::set_visible(bool make_visible)
{
    assert(window_m);

    if (make_visible)
    {
        ::ShowWindow(window_m);
        ::DrawControls(window_m);
    }
    else
    {
        ::HideWindow(window_m);
    }
}

/**************************************************************************************************/

void window_t::set_size(const point_2d_t& size)
{
    assert(window_m);

    if (debounce_m)
        return;

    debounce_m = true;
    
    ::Rect content_bounds;
    ADOBE_REQUIRE_STATUS(::GetWindowBounds(window_m, kWindowContentRgn, &content_bounds));
    
    ::Rect bounds;
    ADOBE_REQUIRE_STATUS(::GetWindowBounds(window_m, kWindowStructureRgn, &bounds));
    
    // Transition Window operates on the global bounds - where as our size is on the content
    
    long additional_width ((bounds.right - bounds.left) - (content_bounds.right - content_bounds.left));
    long additional_height ((bounds.bottom - bounds.top) - (content_bounds.bottom - content_bounds.top));
    
    bounds.right = bounds.left + short(size.x_m) + additional_width;
    bounds.bottom = bounds.top + short(size.y_m) + additional_height;

    dimensions_m = size;

    ::HISize min_size = { static_cast<CGFloat>(size.x_m), static_cast<CGFloat>(size.y_m) };

    ADOBE_REQUIRE_STATUS(::SetWindowResizeLimits(window_m, &min_size, NULL));

    ADOBE_REQUIRE_STATUS(::TransitionWindow(window_m,
                                            kWindowSlideTransitionEffect,
                                            kWindowResizeTransitionAction,
                                            &bounds));

    debounce_m = false;
}

/**************************************************************************************************/

void window_t::reposition(window_reposition_t position)
{
    assert(window_m);

    ::WindowPositionMethod window_position(0);

    if (position == window_reposition_center_s)
        window_position = kWindowCenterOnMainScreen;
    else if (position == window_reposition_alert_s)
        window_position = kWindowAlertPositionOnMainScreen;

    ADOBE_REQUIRE_STATUS(::RepositionWindow(window_m, NULL, window_position));
}

/**************************************************************************************************/

void window_t::monitor_resize(const window_resize_proc_t& proc)
{
    assert(window_m);

    resize_handler_m.callback_m = proc;
}

/**************************************************************************************************/

template <>
platform_display_type insert<window_t>(display_t&               display,
                                                platform_display_type& /*parent*/,
                                                window_t&       element)
{
    assert(!element.window_m);

    static const ::Rect bounds_s = { 0, 0, 20, 20 };

    ::WindowClass      window_class(0);
    ::WindowAttributes window_attributes(kWindowCompositingAttribute); // always use compositing now
    ::WindowModality   window_modality(kWindowModalityNone);

    if (element.style_m == window_style_moveable_modal_s)      window_class = kMovableModalWindowClass;
    else if (element.style_m == window_style_floating_s)       window_class = kFloatingWindowClass;

    if (element.modality_m == window_modality_system_s)      window_modality = kWindowModalitySystemModal;
    else if (element.modality_m == window_modality_app_s)    window_modality = kWindowModalityAppModal;
    else if (element.modality_m == window_modality_window_s) window_modality = kWindowModalityWindowModal;

    if (element.attributes_m & window_attributes_standard_handler_s) window_attributes |= kWindowStandardHandlerAttribute;
    if (element.attributes_m & window_attributes_resizeable_s)       window_attributes |= kWindowResizableAttribute;
    if (element.attributes_m & window_attributes_live_resizeable_s)  window_attributes |= kWindowLiveResizeAttribute;
    if (element.attributes_m & window_attributes_metal_s)            window_attributes |= kWindowMetalAttribute;

    ADOBE_REQUIRE_STATUS(::CreateNewWindow(window_class, window_attributes, &bounds_s, &element.window_m));
    ADOBE_REQUIRE_STATUS(::SetWindowModality(element.window_m, window_modality, NULL));
    ADOBE_REQUIRE_STATUS(::SetWindowTitleWithCFString(element.window_m, explicit_cast<auto_cfstring_t>(element.name_m).get()));
    ADOBE_REQUIRE_STATUS(::GetRootControl(element.window_m, &element.root_control_m));
    ADOBE_REQUIRE_STATUS(::SetThemeWindowBackground(element.window_m, kThemeBrushModelessDialogBackgroundActive, static_cast<bool>(window_attributes & kWindowMetalAttribute)));

    element.resize_handler_m.handler_m.install(element.window_m);

    platform_display_type top(0);

    return display.insert(top, element.root_control_m);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
