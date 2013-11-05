/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_tokens.hpp>
#include <adobe/name.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

static_name_t  key_action              = "action"_name;
static_name_t  key_alt_text            = "alt"_name;
static_name_t  key_bind                = "bind"_name;
static_name_t  key_bind_additional     = "bind_additional"_name;
static_name_t  key_bind_controller     = "bind_controller"_name;
static_name_t  key_bind_group          = "bind_group"_name;
static_name_t  key_bind_output         = "bind_output"_name;
static_name_t  key_bind_units          = "bind_units"_name;
static_name_t  key_bind_view           = "bind_view"_name;
static_name_t  key_cancel              = "cancel"_name;
static_name_t  key_characters          = "characters"_name;
static_name_t  key_class               = "class"_name;
static_name_t  key_contributing        = "contributing"_name;
static_name_t  key_count               = "count"_name;
static_name_t  key_custom_item_name    = "custom_item_name"_name;
static_name_t  key_decimal_places      = "decimal_places"_name;
static_name_t  key_default             = "default"_name;
static_name_t  key_digits              = "digits"_name;
static_name_t  key_display_disable     = "display_disable"_name;
static_name_t  key_domain              = "domain"_name;
static_name_t  key_first               = "first"_name;
static_name_t  key_focus               = "focus"_name;
static_name_t  key_format              = "format"_name;
static_name_t  key_grow                = "grow"_name;
static_name_t  key_horizontal          = "horizontal"_name;
static_name_t  key_identifier          = "identifier"_name;
static_name_t  key_image               = "image"_name;
static_name_t  key_image_on            = "image_on"_name;
static_name_t  key_image_off           = "image_off"_name;
static_name_t  key_image_disabled      = "image_disabled"_name;
static_name_t  key_increment           = "increment"_name;
static_name_t  key_interval_count      = "interval_count"_name;
static_name_t  key_is_indeterminate    = "is_indeterminate"_name;
static_name_t  key_is_relevance        = "is_relevance"_name;
static_name_t  key_items               = "items"_name;
static_name_t  key_last                = "last"_name;
static_name_t  key_lines               = "lines"_name;
static_name_t  key_localization_set    = "localization_set"_name;
static_name_t  key_max_characters      = "max_characters"_name;
static_name_t  key_max_digits          = "max_digits"_name;
static_name_t  key_max_value           = "max_value"_name;
static_name_t  key_metal               = "metal"_name;
static_name_t  key_min_max_filter      = "min_max_filter"_name;
static_name_t  key_min_value           = "min_value"_name;
static_name_t  key_monospaced          = "monospaced"_name;
static_name_t  key_name                = "name"_name;
static_name_t  key_offset_contents     = "offset_contents"_name;
static_name_t  key_orientation         = "orientation"_name;
static_name_t  key_password            = "password"_name;
static_name_t  key_popup_bind          = "popup_bind"_name;
static_name_t  key_popup_placement     = "popup_placement"_name;
static_name_t  key_popup_value         = "popup_value"_name;
static_name_t  key_scale               = "scale"_name;
static_name_t  key_scrollable          = "scrollable"_name;
static_name_t  key_short_name          = "short_name"_name;
static_name_t  key_size                = "size"_name;
static_name_t  key_slider_point        = "slider_point"_name;
static_name_t  key_slider_ticks        = "slider_ticks"_name;
static_name_t  key_target              = "target"_name;
static_name_t  key_touch               = "touch"_name;
static_name_t  key_touched             = "touched"_name;
static_name_t  key_trailing_zeroes     = "trailing_zeroes"_name;
static_name_t  key_units               = "units"_name;
static_name_t  key_value               = "value"_name;
static_name_t  key_value_off           = "value_off"_name;
static_name_t  key_value_on            = "value_on"_name;
static_name_t  key_vertical            = "vertical"_name;
static_name_t  key_wrap                = "wrap"_name;

// New modifer names:
static_name_t  key_modifier_set        = "modifier_set"_name;
static_name_t  key_modifier_option     = "option"_name;
static_name_t  key_modifier_command    = "command"_name;
static_name_t  key_modifier_control    = "control"_name;
static_name_t  key_modifier_shift      = "shift"_name;

// REVISIT = { sparent } : deprecated modifer names.
static_name_t  key_modifiers           = "modifiers"_name;
static_name_t  key_modifiers_cmd       = "cmd"_name;
static_name_t  key_modifiers_ctl       = "ctl"_name;
static_name_t  key_modifiers_ctlcmd    = "ctlcmd"_name;
static_name_t  key_modifiers_default   = "default"_name;
static_name_t  key_modifiers_opt       = "opt"_name;
static_name_t  key_modifiers_optcmd    = "optcmd"_name;
static_name_t  key_modifiers_optctl    = "optctl"_name;
static_name_t  key_modifiers_optctlcmd = "optctlcmd"_name;
// end deprecated modifer names

static_name_t  name_button             = "button"_name;
static_name_t  name_checkbox           = "checkbox"_name;
static_name_t  name_dialog             = "dialog"_name;
static_name_t  name_display_number     = "display_number"_name;
static_name_t  name_edit_number        = "edit_number"_name;
static_name_t  name_edit_text          = "edit_text"_name;
static_name_t  name_group              = "group"_name;
static_name_t  name_image              = "image"_name;
static_name_t  name_label              = "label"_name;
static_name_t  name_link               = "link"_name;
static_name_t  name_list               = "list"_name;
static_name_t  name_message            = "message"_name;
static_name_t  name_optional           = "optional"_name;
static_name_t  name_palette            = "palette"_name;
static_name_t  name_panel              = "panel"_name;
static_name_t  name_popup              = "popup"_name;
static_name_t  name_popup_cluster      = "popup_cluster"_name;
static_name_t  name_progress_bar       = "progress_bar"_name;
static_name_t  name_radio_button       = "radio_button"_name;
static_name_t  name_separator          = "separator"_name;
static_name_t  name_size_group         = "size_group"_name;
static_name_t  name_slider             = "slider"_name;
static_name_t  name_tab_group          = "tab_group"_name;
static_name_t  name_toggle             = "toggle"_name;

/****************************************************************************************************/

} //namespace adobe

/****************************************************************************************************/
