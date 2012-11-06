/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_presets.hpp>
#include <adobe/future/widgets/headers/presets_common.hpp>

#include <adobe/future/macintosh_graphic_utils.hpp>
#include <adobe/future/image_slurp.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/future/modal_dialog_interface.hpp>
#include <adobe/future/resources.hpp>
#include <adobe/future/widgets/headers/display.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/once.hpp>
#include <adobe/virtual_machine.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

pascal void draw_preset_button(ControlRef control, SInt16)
try
{
    typedef adobe::auto_resource< adobe::cg_image_t > auto_cg_image_t;

    adobe::presets_t& presets(*reinterpret_cast<adobe::presets_t*>(::GetControlReference(control)));

    WindowRef    our_window(::GetControlOwner(control));
    CGrafPtr     window_port(::GetWindowPort(our_window));
    CGContextRef context(0);
    bfs::path    preset_name(::IsControlEnabled(control) == false ? "preset_button_u_d.tga" :
                                               presets.selected_m ? "preset_button_d_n.tga" :
                                                                    "preset_button_u_n.tga");

    boost::gil::rgba8_image_t gil_image;

    adobe::image_slurp(preset_name, gil_image);

    auto_cg_image_t image(adobe::make_cg_image(gil_image));
    CGImageRef      preset_icon(adobe::to_CGImageRef(image.get()));
    Rect            bounds = { 0 };
    Rect            window_bounds = { 0 };

    ::GetWindowBounds(our_window, kWindowContentRgn, &window_bounds);

    adobe::implementation::get_bounds(control, bounds, true);

    // Quartz draws the image relative to the bottom-left corner of the window; we offset the image accordingly.

    int    control_height(bounds.bottom - bounds.top);
    int    control_width(bounds.right - bounds.left);
    CGRect where(::CGRectMake(bounds.left, bounds.top - 1,
                              control_width, control_height));

    ADOBE_REQUIRE_STATUS(::QDBeginCGContext(window_port, &context));

    ::CGContextTranslateCTM(context, 0.0, static_cast<float>(window_bounds.bottom - window_bounds.top - 1)); 
    ::CGContextScaleCTM(context, 1.0, -1.0);
    ::CGContextSetShouldAntialias(context, false);

    ::CGContextDrawImage(context, where, preset_icon);

    ::CGContextSynchronize(context);

    ADOBE_REQUIRE_STATUS(::QDEndCGContext(window_port, &context));
}
catch (const adobe::stream_error_t& err)
{
    adobe::report_error(adobe::format_stream_error(err));
}
catch (const std::exception& err)
{
    adobe::report_error(std::string("Exception: ") + err.what());
}
catch (...)
{
    adobe::report_error("Exception: Unknown");
}

/****************************************************************************************************/

void initialize(adobe::presets_t& control,
                ::HIViewRef       /*parent*/)
{
    const ::Rect bounds = { 0, 0, 21, 26 };

    static ::ControlUserPaneDrawUPP draw_handler(::NewControlUserPaneDrawUPP(draw_preset_button));

    ::ADOBE_REQUIRE_STATUS(::CreateUserPaneControl(0, &bounds, 0, &control.control_m));

    ::SetControlReference(control.control_m, reinterpret_cast< ::SInt32 >(&control));

    adobe::implementation::set_widget_data(control.control_m, kControlEntireControl, kControlUserPaneDrawProcTag, draw_handler);

    adobe::implementation::set_theme(control, control.theme_m);

    control.popup_m.monitor(boost::bind(&adobe::presets_t::do_imbue, boost::ref(control), _1));

    if (!control.alt_text_m.empty())
        adobe::implementation::set_control_alt_text(control.control_m, control.alt_text_m);

    control.hit_handler_m.handler_m.install(control.control_m);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

ADOBE_WIDGET_TAG_BOILERPLATE(presets_t, "<xstr id='metric:presets'/>");

/****************************************************************************************************/

::OSStatus presets_hit_handler_t::handle_event(::EventHandlerCallRef /*next*/,
                                               ::EventRef            /*event*/)
{
    OSStatus  result(noErr);

    if (::IsControlEnabled(widget_m.control_m) == false)
        return result;

    ::Rect    bounds = { 0 };
    ::Rect    window_bounds = { 0 };

    widget_m.selected_m = true;
    implementation::set_refresh(widget_m.control_m);
    ::Draw1Control(widget_m.control_m);

    ::GetWindowBounds(::GetControlOwner(widget_m.control_m), kWindowContentRgn, &window_bounds);
    implementation::get_bounds(widget_m.control_m, bounds, true);

    name_t append_presets(implementation::localization_value(widget_m, key_preset_menu_item_append_preset, "Append Preset...").c_str());
    name_t add_preset(implementation::localization_value(widget_m, key_preset_menu_item_add_preset, "Add Preset...").c_str());
    name_t delete_preset(implementation::localization_value(widget_m, key_preset_menu_item_delete_preset, "Delete Preset...").c_str());
#ifndef NDEBUG
    static_name_t separator("-");
    static_name_t resave_presets("(debug) Re-save Presets To File");
#endif

    name_t menu_options[] =
    {
        append_presets,
        add_preset,
        delete_preset,
#ifndef NDEBUG
        separator,
        resave_presets,
#endif
    };

    std::size_t menu_options_size(sizeof(menu_options) / sizeof(menu_options[0]));

    long ch(bounds.bottom - bounds.top);
    long cw(bounds.right - bounds.left);

    name_t menu_result;
    
    if (!implementation::context_menu(::GetControlOwner(widget_m.control_m),
                                             window_bounds.left + bounds.left + cw / 2,
                                             window_bounds.top + bounds.top + ch / 2,
                                             &menu_options[0],
                                             &menu_options[0] + menu_options_size,
                                             menu_result))
    { /* user didn't select anything */ }
    else if (menu_result == append_presets)
    {
        implementation::append_user_preset_set(widget_m);
    }
    else if (menu_result == add_preset)
    {
        // REVISIT (fbrereto) : This needs to reupdate the preset items after this point
        if (widget_m.snapshot_proc_m)
            implementation::append_user_preset(widget_m, widget_m.snapshot_proc_m());
    }
    else if (menu_result == delete_preset)
    {
        // REVISIT (fbrereto) : This needs to reupdate the preset items after this point
        implementation::delete_user_preset(widget_m);
    }
#ifndef NDEBUG
    else if (menu_result == resave_presets)
    {
        implementation::save_user_preset_set(widget_m);
    }
#endif
    else
    { /* do nothing */ }

    widget_m.selected_m = false;
    implementation::set_refresh(widget_m.control_m);
    ::Draw1Control(widget_m.control_m);

    return result;
}

/****************************************************************************************************/

presets_t::presets_t(const std::string&         name,
                     const std::string&         domain,
                     const std::string&         alt_text,
                     const array_t&      bind_set,
                     const dictionary_t& localization_set,
                     theme_t                    theme) :
    control_m(0),
    category_popup_m(implementation::localization_value(localization_set, key_preset_category_popup_name, "Category:"),
                     implementation::localization_value(localization_set, key_preset_category_popup_alt_text, "Select a category of presets for this dialog"),
                     implementation::localization_value(localization_set, key_preset_custom_category_name, "Custom"),
                     0, 0, theme),
    popup_m(implementation::localization_value(localization_set, key_preset_preset_popup_name, "Preset:"),
            implementation::localization_value(localization_set, key_preset_preset_popup_alt_text, "Select a preset for settings in this dialog"),
            implementation::localization_value(localization_set, key_preset_custom_category_name, "Custom"),
            0, 0, theme),
    hit_handler_m(*this),
    bind_set_m(bind_set),
    name_m(name),
    domain_m(domain),
    alt_text_m(alt_text),
    selected_m(false),
    localization_set_m(localization_set),
    type_2_debounce_m(false),
    custom_m(false)
{ }

/****************************************************************************************************/

void presets_t::measure(extents_t& result)
{
    assert(control_m);

    popup_m.measure(result);

    result.width() += 4 + 26; // gap + icon
    result.height() = std::max(result.height(), 21);

    extents_t cat_result;

    category_popup_m.measure(cat_result);

    result.width() = std::max(result.width(), cat_result.width());

    // REVISIT (fbrereto) : This presumes the popups are of the same height.

    popup_height_m = cat_result.height();

    result.height() += popup_height_m + 4;

    if (!result.horizontal().guide_set_m.empty() && !cat_result.horizontal().guide_set_m.empty())
        result.horizontal().guide_set_m[0] = std::max(result.horizontal().guide_set_m[0],
                                                      cat_result.horizontal().guide_set_m[0]);
}

/****************************************************************************************************/

void presets_t::place(const place_data_t& place_data)
{
    assert(control_m);

    place_data_t category_place(place_data);
    place_data_t popup_place(place_data);
    place_data_t icon_place(place_data);

    // set up the top (category) popup
    width(category_place) -= 4 + 26;
    height(category_place) = popup_height_m;

    // set up the bottom popup
    top(popup_place) = bottom(place_data) - popup_height_m;
    width(popup_place) -= 4 + 26;
    height(popup_place) = popup_height_m;

    // set up the icon
    width(icon_place) = 26;
    height(icon_place) = 21;
    left(icon_place) += width(popup_place) + 4;

    popup_m.place(popup_place);
    category_popup_m.place(category_place);

    implementation::set_bounds(*this, icon_place);
}

/****************************************************************************************************/

void enable(presets_t& control, bool make_enabled)
{
    assert(control.control_m);

    implementation::set_active(control.control_m, make_enabled);
}

/****************************************************************************************************/

void monitor(presets_t& control, presets_t::setter_type proc)
{
    assert(control.control_m);

    control.hit_handler_m.setter_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<presets_t>(display_t&             display,
                                               platform_display_type& parent,
                                               presets_t&      element)
{
    insert(display, parent, element.popup_m);
    insert(display, parent, element.category_popup_m);

    initialize(element, parent);

    return display.insert(parent, element.control_m);
}              

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

boost::filesystem::path preset_directory(const presets_t& control)
{
    ::FSRef prefs_folder;

    // first, get the local user's preferences folder
    ADOBE_REQUIRE_STATUS(::FSFindFolder(kUserDomain, kPreferencesFolderType, true, &prefs_folder));

    // convert the preferences folder to a filesystem path
    boost::filesystem::path pref_folder_path(implementation::fsref_to_path(prefs_folder));
    boost::filesystem::path corp_folder_path(pref_folder_path /
                                             implementation::localization_value(control, key_preset_top_folder_name, "Adobe"));

    boost::filesystem::create_directory(corp_folder_path);

    // finalize the resulting boost::filesystem::path
    boost::filesystem::path result(corp_folder_path / control.domain_m);

    boost::filesystem::create_directory(result);

    return result;
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
