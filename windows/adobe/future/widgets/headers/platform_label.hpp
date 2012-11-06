/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/


#ifndef ADOBE_LABEL_T_HPP
#define ADOBE_LABEL_T_HPP


#include <windows.h>

#include <string>

#include <boost/noncopyable.hpp>

#include <adobe/memory.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/widget_attributes.hpp>
#include <boost/noncopyable.hpp>





namespace adobe {
    struct label_t  : boost::noncopyable {

        label_t(const std::string& name, 
                const std::string& alt_text, 
                std::size_t        characters,
                theme_t            theme
                );

        ~label_t();

        HWND        window_m;
        theme_t     theme_m;
        WNDPROC     default_window_proc_m;
        std::string name_m;
        std::string alt_text_m;
        std::size_t characters_m;
    };

    void               initialize(label_t& value, HWND parent);


    void measure(label_t& value, extents_t& result);
    
    void measure_vertical(label_t& value, extents_t& calculated_horizontal, 
        const place_data_t& placed_horizontal);       

    void place(label_t& value, const place_data_t& place_data);
    

    void enable(label_t& value, bool make_enabled);

    void measure_label_text(const label_t& label, extents_t& result, HWND temp_parent);

    extents_t measure_text(const std::string& text, theme_t theme, HWND temp_parent);

    std::string get_control_string(const label_t& widget);

    inline HWND get_display(const label_t& widget)
        { return widget.window_m; }

} // namespace adobe


#endif

/****************************************************************************************************/
