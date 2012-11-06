/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_METRICS_EXTRACTOR_MAC_HPP
#define ADOBE_METRICS_EXTRACTOR_MAC_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/array.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

/*!
\defgroup widget_metrics_mac Mac Widget Metrics
\ingroup apl_widgets

\section widget_metrics_mac_parameters Parameters
The following is a list of metrics one can supply for a widget on the Mac.
All values are in unit pixels. All values default to 0.

<table width="100%" border="1">
    <tr><th width="10%">Name</th><th width="30%">Structure</th><th>Notes</th></tr>
    <tr>
        <td><code>size</code></td>
        <td><code>[ width, height ]</code></td>
        <td>Base size of the widget</td>
    </tr>
    <tr>
        <td><code>adjust_position</code></td>
        <td><code>[ left, top ]</code></td>
        <td>Number of pixels to push the widget. Positive values move the
        widget away from the origin of the window.</td>
    </tr>
    <tr>
        <td><code>adjust_size</code></td>
        <td><code>[ width, height ]</code></td>
        <td>Adjustments made to the base size of the widget. The adjusted
        size is passed to the OS so the resulting widget is the base size
        originally intended.</td>
    </tr>
    <tr>
        <td><code>adjust_baseline</code></td>
        <td><code>value</code></td>
        <td>Adjustment of the baseline. Positive values move the baseline
        away from the origin of the window.</td>
    </tr>
    <tr>
        <td><code>outset</code></td>
        <td><code>[ left, top, right, bottom ]</code></td>
        <td>Amount of outset for a given edge of the widget. These amounts
        affect the base size and position of the widget when the bounds of
        the widget are set. See \ref widget_metrics_mac_pipeline_placement.</td>
    </tr>
    <tr>
        <td><code>frame</code></td>
        <td><code>[ left, top, right, bottom ]</code></td>
        <td>Amount of frame for a given edge of the widget.</td>
    </tr>
    <tr>
        <td><code>inset</code></td>
        <td><code>[ left, top, right, bottom ]</code></td>
        <td>Amount of inset for a given edge of the widget.</td>
    </tr>
    <tr>
        <td><code>spacing</code></td>
        <td><code>value</code></td>
        <td>If there is text in the widget, this is the spacing needed between
        the widget's visual element and the text for the widget. Example:
        <code>check_box</code> has a nonzero spacing, but <code>button</code>
        has a spacing of 0 (because the text overlays the visual element).</td>
    </tr>
</table>

\section widget_metrics_mac_pipelines Metrics Pipelines

\subsection widget_metrics_mac_pipeline_measurement Measurement Pipeline
    -# All the possible text strings for the widget are measured. The
    largest width, height and baseline are preserved on a per-element basis (i.e., it is
    possible for one piece of text to contribute the max width and another
    to contribute the max height)
    -# The base height of the widget is set to <code>size.height - adjust_size.height</code>
    -# The base width of the widget is set to <code>size.width - adjust_size.width</code>
    -# If text width is nonzero then <code>text_width + spacing</code> is
    added to the base width of the widget
    -# If there is a valid text baseline value the baseline of the result is set
    to <code>text_baseline + adjust_baseline</code>
    -# The <code>outset.left</code> is added to the resultant left outset
    -# The <code>outset.left + outset.right</code> is <i>subtracted</i> from the resultant right outset
    -# The <code>outset.top</code> is added to the resultant top outset
    -# The <code>outset.top + outset.bottom</code> is <i>subtracted</i> from the resultant bottom outset
    -# Resultant frame is set from <code>frame</code>
    -# Resultant inset is set from <code>inset</code>
    -# The resultant extents is returned

\subsection widget_metrics_mac_pipeline_placement Placement Pipeline
    -# <code>adjust_position.left - extents.outset.left</code> is added to
    the left position of the widget
    -# <code>adjust_position.top - extents.outset.top</code> is added to
    the top position of the widget
    -# The height of the widget is increased by
    <code>adjust_size.height + extents.outset.top + extents.outset.bottom</code>
    -# The width of the widget is increased by
    <code>adjust_size.width + extents.outset.left + extents.outset.right</code>
    -# The bounds of the widget are set
*/

/*!
\ingroup apl_widgets

\brief Utility class to access widget metrics for the Macintosh.

metric_extractor_t is a struct that allows for easy access to the metrics found in a parsed and 
evaluated xstr definition from the mac metrics library. See \ref widget_metrics_mac for more
information on the dictionary format describing the metrics for a given widget.
*/

struct metric_extractor_t
{
    /// indices used to access elements for a compound (array-based) metric
    enum array_index_t
    {
            /// first element
        index_left = 0,
            /// second element
        index_top = 1,
            /// third element
        index_right = 2,
            /// fourth element
        index_bottom = 3,
            /// same as index_left
        index_width = index_left,
            /// same as index_top
        index_height = index_top
    };

    /// \param dictionary   the dictionary containing the widget metrics.
    ///                     Internally the dictionary is stored by value.
    explicit metric_extractor_t(const dictionary_t& dictionary = dictionary_t()) :
        dictionary_m(dictionary)
    { }

    /// Obtains a singleton metric
    /// \param tag the name of the metric we are interested in getting
    /// \return the value of the metric; 0 if the metric does not exist
    long operator () (const name_t& tag) const
    {
        long result(0);

        if (empty())
            return result;

        get_value(dictionary_m, tag, result);

        return result;
    }

    /// Obtains a singleton metric from a compound (array-based) metric
    /// \param tag the name of the metric we are interested in getting
    /// \param index the index inside the compound metric to fetch
    /// \return the value of the metric; 0 if the metric does not exist
    long operator () (const name_t& tag, array_index_t index) const
    {
        if (empty()) return 0;
            
        dictionary_t::const_iterator i = dictionary_m.find(tag);
        if (i == dictionary_m.end()) return 0;
        
        const array_t& array_result = i->second.cast<array_t>();
        
        if (!(std::size_t(index) < array_result.size())) return 0;
        
        return array_result[std::size_t(index)].cast<long>();
    }

    /// If this extractor hasn't been setup yet or has no metrics stored within,
    /// then this function returns true, otherwise it returns false.
    /// \return does this extractor have metrics available
    bool empty() const { return dictionary_m.empty(); }

    friend inline bool operator==(const metric_extractor_t& x, const metric_extractor_t& y)
    { return x.dictionary_m == y.dictionary_m; }
    
    metric_extractor_t&  operator=(const metric_extractor_t& x) 
    {
        dictionary_m = x.dictionary_m;
        return *this;
    }

private:
    dictionary_t dictionary_m;
};

/****************************************************************************************************/

}

/****************************************************************************************************/

#endif

/****************************************************************************************************/
