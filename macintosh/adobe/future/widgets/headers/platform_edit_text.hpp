/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_EDIT_TEXT_HPP
#define ADOBE_WIDGET_EDIT_TEXT_HPP

/****************************************************************************************************/

#include <boost/operators.hpp>

#include <adobe/future/macintosh_events.hpp>

#include <adobe/config.hpp>
#include <adobe/extents.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>

#include <adobe/future/widgets/headers/macintosh_focus_handler.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/edit_text_common.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
struct edit_text_t;

typedef std::string                                                edit_text_model_type;
typedef boost::function<void (const edit_text_model_type&, bool&)> edit_text_pre_edit_proc_t;
typedef boost::function<void (const edit_text_model_type&)>        edit_text_post_edit_proc_t;

/****************************************************************************************************/

struct edit_text_handler_t
{
    explicit edit_text_handler_t(edit_text_t& widget) :
        widget_m(widget)
    {
        handler_m.insert(kEventClassCommand, kHICommandFromMenu);
        handler_m.insert(kEventClassTextInput, kEventTextInputUnicodeForKeyEvent);

        handler_m.monitor_proc_m = boost::bind(&edit_text_handler_t::handle_event,
                                               boost::ref(*this), _1, _2);
    }

    ::OSStatus handle_event(::EventHandlerCallRef next,
                            ::EventRef            event);

    event_handler_t handler_m;
    edit_text_t&    widget_m;
};
#endif
/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Edit text widget
*/
struct edit_text_t : boost::equality_comparable<edit_text_t>
{
    /// model type for this widget
    typedef edit_text_model_type       model_type;

    /// monitor proc callback type for this widget
    typedef edit_text_post_edit_proc_t setter_type;

    /// constructor. Takes an edit_text_ctor_block_t, which is a huge block-o-data
    explicit edit_text_t(const edit_text_ctor_block_t& block);

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
    void display(const model_type& text);
    ///@}

    /*!
        @name Controller Concept Operations
        @{

        See the \ref concept_controller concept and \ref controller.hpp for more information.
    */
    void monitor(const setter_type& proc);

    void enable(bool make_active);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    void        set_selection(long start_char, long end_char);
    void        signal_pre_edit(const edit_text_pre_edit_proc_t& proc);

    label_t&       get_label()       { assert(name_m.get()); return *name_m; }
    const label_t& get_label() const { assert(name_m.get()); return *name_m; }

    ::ControlRef                 control_m;
    ::HIViewRef                  scroll_control_m;
    boost::scoped_ptr<label_t>   name_m;
    std::string                  alt_text_m;
    theme_t                      theme_m;
    mutable metric_extractor_t   metrics_m;
    bool                         type_2_debounce_m;
    model_type                   type_1_debounce_m;
    bool                         using_label_m;
    bool                         scrollable_m;
    bool                         password_m;
    long                         rows_m;
    long                         cols_m;
    long                         max_cols_m;
    long                         static_width_m;
    long                         static_height_m;
    long                         static_baseline_m;
    long                         edit_height_m;
    long                         edit_baseline_m;
    mouse_in_out_event_handler_t mouse_handler_m;
    edit_text_handler_t          edit_text_handler_m;
    edit_text_pre_edit_proc_t    pre_edit_proc_m;
    edit_text_post_edit_proc_t   post_edit_proc_m;
    focus_handler_t              focus_handler_m;
#endif
};

/****************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
inline ::HIViewRef get_display(const edit_text_t& widget)
    { return widget.scrollable_m ? widget.scroll_control_m : widget.control_m; }
#endif
/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

//ADOBE_WIDGET_EDIT_TEXT_HPP
#endif

/****************************************************************************************************/

