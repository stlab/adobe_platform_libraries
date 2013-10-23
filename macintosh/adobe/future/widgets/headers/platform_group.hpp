/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_WIDGET_GROUP_HPP
#define ADOBE_WIDGET_GROUP_HPP

/****************************************************************************************************/

#include <adobe/extents.hpp>
#include <adobe/eve.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/widget_attributes.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!
    \ingroup apl_widgets_carbon

    \brief Group container widget

    \model_of
        - \ref concept_placeable

    The semantics of a group are that of a flag. One would use a group when you
    want to have a selectable option turned on or off. There is a third state the
    group can be in, the undetermined state, which is neither true or false. This
    is often represented with a dash or a filled group.
*/
struct group_t
{
    /*!
        \param name is the name of the widget
        \param alt_text is additional help text for the widget when the user pauses over it
        \param theme is the theme for the widget
    */
    group_t(const std::string& name,
            const std::string& alt_text,
            theme_t            theme);

    /*!
        @name Placeable Concept Operations
        @{

        See the \ref concept_placeable concept and \ref placeable.hpp for more information.
    */
    void measure(extents_t& result);

    void place(const place_data_t& place_data);
    ///@}

#ifndef ADOBE_NO_DOCUMENTATION
    mutable metric_extractor_t metrics_m;
    ::ControlRef               control_m;
    std::string                name_m;
    std::string                alt_text_m;
    theme_t                    theme_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
