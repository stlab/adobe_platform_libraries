/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_WIDGET_DISPLAY_NUMBER_HPP
#define ADOBE_WIDGET_DISPLAY_NUMBER_HPP

/**************************************************************************************************/

#include <adobe/eve.hpp>
#include <adobe/future/widgets/headers/macintosh_metric_extractor.hpp>
#include <adobe/future/widgets/headers/number_unit.hpp>
#include <adobe/future/widgets/headers/widget_utils.hpp>
#include <adobe/macintosh_carbon_safe.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
    \ingroup apl_widgets_carbon

    \brief Display number view

    The semantics of a display number view are similar to that of a
    traditional label (aka static text). The two exceptions are these:
    first, the display_number view models a \ref concept_view, so it
    can be bound to the property model as such. Second, it has the
    ability to format the value received from the property model before
    displaing it to the user. This gives the display_number view the
    ability to conform the value coming from the property model into
    something the user is more able to understand.

    The display_number_t view models the two-pass Placeable concept.
    This means that its height is measured after its width has been
    derived by the layout engine.

    \model_of
        - \ref concept_view
        - \ref concept_placeable
*/
struct display_number_t : boost::noncopyable
{
    /*! The model type for this view */
	typedef double model_type;

    typedef std::vector<unit_t> unit_set_t;

	template <class ForwardIterator>
	display_number_t(const std::string& name,
					 const std::string& alt_text,
					 ForwardIterator    first,
					 ForwardIterator    last, 
					 theme_t            theme,
			 		 int                characters);

#ifndef ADOBE_NO_DOCUMENTATION
    auto_control_t             control_m;
    mutable metric_extractor_t metrics_m;
    std::string                name_m;
    std::string                alt_text_m;
	unit_set_t      		   unit_set_m;
	theme_t                    theme_m;
	std::string::size_type     label_chars_m;
    int                        characters_m;
#endif
};

/**************************************************************************************************/

/*!
    \param name is the name of the view. Typically shown as a label

    \param alt_text is additional information to be shown to the
    user when they hover over the view with the mouse.

    \param first is the first unit_t decorator for this view

    \param last is one-past-the-last unit_t decorator for this view

    \param theme details additional visual attributes for the view

    \param characters is the minimum width of this placeable view, not including its name.
*/
template <typename ForwardIterator>
display_number_t::display_number_t(const std::string& name,
                                   const std::string& alt_text,
                                   ForwardIterator    first,
                                   ForwardIterator    last, 
                                   theme_t            theme,
                                   int                characters) :
    name_m(name),
    alt_text_m(alt_text),
    unit_set_m(first, last),
    theme_m(theme),
    label_chars_m(0),
    characters_m(characters)
{ }
	
/**************************************************************************************************/

/*!
    \relates display_number_t

    @name PlaceableTwopass Concept Operations
    @{

    See the \ref concept_placeable_twopass concept and \ref placeable.hpp for more information.
*/
void measure(display_number_t& value, extents_t& result);
    
void measure_vertical(display_number_t&   value,
                      extents_t&          calculated_horizontal, 
                      const place_data_t& placed_horizontal);

void place(display_number_t& value, const place_data_t& place_data);
///@}

/*!
    \relates display_number_t

    @name View Concept Operations
    @{

    See the \ref concept_view concept and \ref view.hpp for more information.
*/
void display(display_number_t& value, const display_number_t::model_type&);
///@}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
