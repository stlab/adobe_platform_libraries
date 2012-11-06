/*
    Copyright 2006-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

// This is the implementation of the class which looks up widget metrics on
// Windows systems. It has two implementations, one uses UxTheme to look up
// widget metrics, and the other uses constant values (and is used on systems
// where UxTheme is unavailable or Visual Styles are disabled).

/****************************************************************************************************/

#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>

#include <sstream>
#include <stdexcept>
#include <tchar.h>
#include <cassert>
#include <cstring>

#include <adobe/once.hpp>

#include <boost/static_assert.hpp>

#include <uxtheme.h>
//Yuck:
#include <tmschema.h>
#define SCHEME_STRINGS 1
#include <tmschema.h>
//End yuck

/****************************************************************************************************/

ADOBE_ONCE_DECLARATION(windows_theme_metrics_once)

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

//
// Technically all of these calls are supposed to use a state parameter. We are
// only obtaining metrics, rather than drawing, so we don't actually need to
// specify any particular state (as metrics should be invariant over all states).
//
// According to the tmschema.h (in the Platform SDK) a state of 1 should always
// work. Thus, we will use this enum below whenever an API requires a state.

enum { kState = 1 };

/****************************************************************************************************/

struct uxtheme_param_block_t
{
public:
    uxtheme_param_block_t();
    ~uxtheme_param_block_t();

public:
    //
    // Because we open UxTheme.dll dynamically we need to keep
    // function pointers into the library. These are those.
    //
    typedef HTHEME  (__stdcall *OpenThemeData_t)(HWND hwnd, LPCWSTR pszClassList);
    typedef HRESULT (__stdcall *CloseThemeData_t)(HTHEME hTheme);
    typedef HRESULT (__stdcall *GetThemeMargins_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, RECT* prc, MARGINS* pMargins);
    typedef HRESULT (__stdcall *GetThemePartSize_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, RECT* pRect, enum THEMESIZE eSize, SIZE* psz);
    typedef HRESULT (__stdcall *GetThemeInt_t)(HTHEME hTheme, int iPartId, int iStateId, int iPropId, int* piVal);
    typedef HRESULT (__stdcall *GetThemeTextExtent_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, const RECT* pBoundingRect, RECT* pExtentRect);
    typedef HRESULT (__stdcall *GetThemeTextMetrics_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, TEXTMETRIC* ptm);
    typedef HRESULT (__stdcall *GetThemeFont_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW* pFont);
    typedef HRESULT (__stdcall *GetThemeSysFont_t)(HTHEME hTheme, int iFontId, LOGFONTW* pFont);
    typedef HRESULT (__stdcall *DrawThemeParentBackground_t)(HWND window, HDC dc, LPRECT prc);
    typedef BOOL    (__stdcall *IsThemeActive_t)(void);

    OpenThemeData_t             OpenThemeDataPtr;
    CloseThemeData_t            CloseThemeDataPtr;
    GetThemeMargins_t           GetThemeMarginsPtr;
    GetThemePartSize_t          GetThemePartSizePtr;
    GetThemeInt_t               GetThemeIntPtr;
    GetThemeTextExtent_t        GetThemeTextExtentPtr;
    GetThemeTextMetrics_t       GetThemeTextMetricsPtr;
    GetThemeFont_t              GetThemeFontPtr;
    GetThemeSysFont_t           GetThemeSysFontPtr;
    IsThemeActive_t             IsThemeActivePtr;
    DrawThemeParentBackground_t DrawThemeParentBackgroundPtr;

    HMODULE theme_dll_m; // Handle to the UxTheme.DLL.
    HTHEME  theme_m;     // Handle to the theme which we are currently using.
    bool    loaded_m;    // We only ever try to load UxTheme.DLL once, this boolean tells us if we were successful.
    HWND    window_m;    // The HWND we are currently inspecting.
};

/*************************************************************************************************/

uxtheme_param_block_t::uxtheme_param_block_t() :
    OpenThemeDataPtr(0),
    CloseThemeDataPtr(0),
    GetThemeMarginsPtr(0),
    GetThemePartSizePtr(0),
    GetThemeIntPtr(0),
    GetThemeTextExtentPtr(0),
    GetThemeTextMetricsPtr(0),
    GetThemeFontPtr(0),
    GetThemeSysFontPtr(0),
    IsThemeActivePtr(0),
    DrawThemeParentBackgroundPtr(0),
    theme_dll_m(0),
    theme_m(0),
    loaded_m(false),
    window_m(0)
{
    //
    // Try to load the UxTheme library, if we can.
    //
    theme_dll_m = LoadLibrary(__T("UxTheme.dll"));

    if (!theme_dll_m)
        return;

    //
    // Load the addresses of the UxTheme functions.
    //
    if (!(OpenThemeDataPtr = (OpenThemeData_t)GetProcAddress(theme_dll_m, "OpenThemeData"))) return;
    if (!(CloseThemeDataPtr = (CloseThemeData_t)GetProcAddress(theme_dll_m, "CloseThemeData"))) return;
    if (!(GetThemeMarginsPtr = (GetThemeMargins_t)GetProcAddress(theme_dll_m, "GetThemeMargins"))) return;
    if (!(GetThemePartSizePtr = (GetThemePartSize_t)GetProcAddress(theme_dll_m, "GetThemePartSize"))) return;
    if (!(GetThemeIntPtr = (GetThemeInt_t)GetProcAddress(theme_dll_m, "GetThemeInt"))) return;
    if (!(GetThemeTextExtentPtr = (GetThemeTextExtent_t)GetProcAddress(theme_dll_m, "GetThemeTextExtent"))) return;
    if (!(GetThemeTextMetricsPtr = (GetThemeTextMetrics_t)GetProcAddress(theme_dll_m, "GetThemeTextMetrics"))) return;
    if (!(GetThemeFontPtr = (GetThemeFont_t)GetProcAddress(theme_dll_m, "GetThemeFont"))) return;
    if (!(GetThemeSysFontPtr = (GetThemeSysFont_t)GetProcAddress(theme_dll_m, "GetThemeSysFont"))) return;
    if (!(IsThemeActivePtr = (IsThemeActive_t)GetProcAddress(theme_dll_m, "IsThemeActive"))) return;
    if (!(DrawThemeParentBackgroundPtr = (DrawThemeParentBackground_t)GetProcAddress(theme_dll_m, "DrawThemeParentBackground"))) return;

    //
    // All loaded -- we note the success
    //
    loaded_m = true;
}

/*************************************************************************************************/

uxtheme_param_block_t::~uxtheme_param_block_t()
{
    if (loaded_m)
        ::FreeLibrary(theme_dll_m);
}

/*************************************************************************************************/

// This is a global that stores all the necessary links to the uxtheme.dll. If you are in a proc
// and using_styles() is true, you are guaranteed that theme_g is valid and visual styles are
// currently enabled for the application.

uxtheme_param_block_t* theme_g = 0;

/*************************************************************************************************/

void init_windows_theme_metrics_once()
{
    static uxtheme_param_block_t uxtheme_param_block_s;

    theme_g = &uxtheme_param_block_s;
}

/****************************************************************************************************/

std::wstring& target()
{
    static std::wstring target_s;

    return target_s;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace metrics {

/****************************************************************************************************/

extents_t measure_text(const std::string& text, HWND window, int platform_placeable_type)
{
    assert(window);

    extents_t result;

    metrics::set_window(window);

    //
    // If we don't have the type of this widget, then we should return a
    // zero sized rectangle. This is usually correct, and a good assumption
    // anyway.
    //
    if (platform_placeable_type == 0)
        return result;
    //
    // Discover the size of the widget.
    //
    SIZE widget_size;
    bool have_size(get_size(platform_placeable_type, TS_DRAW, widget_size));

    //
    // Discover any margins of this widget.
    //
    MARGINS widget_margins;
    bool    have_margins(get_margins(platform_placeable_type, widget_margins));

    //
    // Get the text metrics (and calculate the baseline of this widget)
    //
    TEXTMETRIC widget_tm;
    bool       have_tm(get_font_metrics(platform_placeable_type, widget_tm));

    //
    // We need the text dimensions to figure out what the width of the widget should
    // be.
    //
    RECT text_extents;
    bool have_extents(get_text_extents(platform_placeable_type,
                                       hackery::convert_utf(text.c_str()),
                                       text_extents));

    //
    // Get any border the widget may have.
    //
    int  border;
    bool have_border(get_integer(platform_placeable_type, TMT_BORDERSIZE, border));

    //
    // Now we can calculate the size we want to return.
    //
    return compose_measurements(have_size    ? &widget_size    : 0,
                                have_margins ? &widget_margins : 0,
                                have_tm      ? &widget_tm      : 0,
                                have_extents ? &text_extents   : 0,
                                have_border  ? &border         : 0);
}

/****************************************************************************************************/

extents_t measure(HWND window, int platform_placeable_type)
{
    return measure_text(implementation::get_window_title(window), window, platform_placeable_type);
}

/****************************************************************************************************/

extents_t compose_measurements(const SIZE*       widget_size,
                                      const MARGINS*    widget_margins,
                                      const TEXTMETRIC* font_metrics,
                                      const RECT*       text_extents,
                                      const int*        border)
{
    extents_t result;

    //
    // First we need to try and calculate the baseline of the text in the widget.
    //
    if (font_metrics)
    {
        //
        // Now we can calculate the baseline of this widget. There are two cases:
        //
        //  (a) The text height is larger or the same as the widget height.
        //  (b) The text height is smaller than the widget height.
        //
        // The height which we return as our best bound is the maximum of the
        // text height and the widget height (with margins added).
        //
        // In case (a) the baseline (measured from the top) is the ascent of the
        // text plus the top margin. In case (b) the baseline is the ascent of the
        // text plus the top margin, plus (widget height - text height)/2.
        //
        long baseline(font_metrics->tmAscent);

        if (widget_margins)
            baseline += widget_margins->cyTopHeight;

        //
        // Deal with case (b).
        //
        if (widget_size)
        {
            long widget_height(widget_size->cy);
            long text_height(font_metrics->tmHeight);

            baseline += (std::max)((long)0, (widget_height - text_height) / 2);
        }

        //
        // The border runs all the way around a widget (on all sides), so we have
        // to add any border to the baseline.
        //
        if (border)
            baseline += *border;

        //
        // Store the baseline away in our result.
        //
        result.slice_m[extents_slices_t::vertical].guide_set_m.push_back(baseline);
    }

    //
    // Now we can calculate the size we want to return.
    //
    if (widget_size)
    {
        result.width() += widget_size->cx;
        result.height() += widget_size->cy;
    }

    if (text_extents)
        result.width() += text_extents->right - text_extents->left;

    if (font_metrics)
        result.height() = std::max<int>(result.height(), font_metrics->tmHeight);

    if (widget_margins)
    {
        result.width() += widget_margins->cxLeftWidth + widget_margins->cxRightWidth;
        result.height() += widget_margins->cyTopHeight + widget_margins->cyBottomHeight;
    }

    if (border)
    {
        result.width() += *border * 2;
        result.height() += *border * 2;
    }

    return result;
}

/****************************************************************************************************/

bool using_styles()
{
    ADOBE_ONCE_INSTANCE(windows_theme_metrics_once);

    return theme_g->loaded_m && hackery::cast<bool>(theme_g->IsThemeActivePtr());
}

/****************************************************************************************************/

bool set_window(HWND window)
{
    enum { class_name_max_size = 512 };

    WCHAR class_name_buf[class_name_max_size];

    if (!GetClassNameW(window, class_name_buf, class_name_max_size))
        return false;

    if (using_styles())
        theme_g->window_m = window; // Keep this window around.

    set_theme_name(class_name_buf);

    return !using_styles() || theme_g->theme_m != 0;
}

/****************************************************************************************************/

void set_theme_name(const WCHAR* in_theme_name)
{
    target() = in_theme_name;

    if (!using_styles())
        return;

    // Close any already open theme data, and load up the new data.
    theme_g->CloseThemeDataPtr(theme_g->theme_m);

    // The next section of code tries to account for the fact that the names required
    // by OpenThemeData are different from the "class name" used by CreateWindowEx
    // I am having trouble finding definitive documentation on these names, so I am 
    // not going to clean this up and break it out into a reusable conversion function yet.
    // MM - 11/15/2005

    static const wchar_t* class_names[] = {
        L"ComboBoxEx32",
        L"SysTabControl32",
        L"SysTreeView32",
        L"msctls_progress32",
        L"msctls_trackbar32",
        L"msctls_updown32",
        L"tooltips_class32",
        L"Static",
        //L"Button",
        //L"ComboBox",
        //L"Edit",
        //L"ListBox",
        //L"NativeFontCtl",
        //L"ScrollBar",
        //L"SysAnimate32",
        //L"SysDateTimePick32",
        //L"SysIPAddress32",
        //L"SysListView32",
        //L"SysMonthCal32",
        //L"SysPager",
        //L"msctls_hotkey32",
    };
    static const wchar_t* theme_names[] = {
        L"ComboBox",
        L"Tab",
        L"TreeView",
        L"Progress",
        L"Trackbar",
        L"Spin",
        L"Tooltip",
        L"Edit",
        //L"Button",
        //L"Globals",
        //L"Header",
        //L"Menu",
        //L"Rebar",
        //L"Scrollbar",
        //L"Status",
        //L"Toolbar",
        //L"Window"  
    };

    BOOST_STATIC_ASSERT(sizeof(class_names) == sizeof(theme_names));

    const wchar_t*      theme_name = target().c_str();
    const ptrdiff_t     names_count = sizeof(class_names) / sizeof(wchar_t*);
    const wchar_t**     i = class_names;

    while(i != &class_names[names_count] && std::wcscmp(*i, theme_name) != 0)
        ++i;

    if(i != &class_names[names_count])
        theme_name = theme_names[i - class_names];

    if (theme_g->window_m)
        theme_g->theme_m = theme_g->OpenThemeDataPtr(theme_g->window_m, theme_name);     
}

/****************************************************************************************************/

bool get_font(int widget_type, LOGFONTW& out_font)
{
    if (using_styles())
    {
        // We need ensure that we have a handle to the theme before continuing.
        if (!theme_g->theme_m)
            return false;

        if (widget_type != -1)
        {
            HDC     tmp_dc = ::GetDC(0);
            HRESULT hr = theme_g->GetThemeFontPtr(theme_g->theme_m, tmp_dc, widget_type, kState, TMT_FONT, &out_font);

            ::ReleaseDC(0,tmp_dc);

            if (hr == S_OK)
                return true;
        }

        //
        // This widget is a text label, most likely.
        // Give it the standard message box font.
        //
        return theme_g->GetThemeSysFontPtr(theme_g->theme_m, TMT_MSGBOXFONT, &out_font) == S_OK;
    }
    else
    {
        //
        // On plain windows everything uses DEFAULT_GUI_FONT.
        //
        HGDIOBJ font(::GetStockObject(DEFAULT_GUI_FONT));

        if (!font)
            return false;

        //
        // Extract a LOGFONTW from the HFONT.
        //
        int rv(::GetObject(font, sizeof(out_font), hackery::cast<LPVOID>(&out_font)));

        ::DeleteObject(font);

        return (rv != 0);
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_font_metrics(int widget_type, TEXTMETRIC& out_metrics)
{
    if (using_styles())
    {
        HDC tmp_dc = ::GetDC(0);

        bool have_metrics = S_OK == theme_g->GetThemeTextMetricsPtr(theme_g->theme_m, tmp_dc, widget_type, kState, &out_metrics);

        ::ReleaseDC(0, tmp_dc);

        return have_metrics;
    }
    else
    {
        //
        // Create the font and select it into a temporary device context.
        //
        LOGFONTW logical_font = { 0 };
        HGDIOBJ  font = 0;
        HDC      tmp_dc = ::GetDC(0);

        if (get_font(widget_type, logical_font))
            font = (HGDIOBJ)::CreateFontIndirectW(&logical_font);

        if (!font)
            font = GetStockObject(DEFAULT_GUI_FONT);

        //
        // Get the metrics.
        //
        HGDIOBJ old_font = ::SelectObject(tmp_dc, font);
        BOOL    have_metrics = ::GetTextMetrics(tmp_dc, &out_metrics);

        //
        // Clean up.
        //
        ::DeleteObject(::SelectObject(tmp_dc, old_font));
        ::ReleaseDC(0, tmp_dc);

        return hackery::cast<bool>(have_metrics);
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_text_extents(int widget_type, std::wstring text, RECT& out_extents, const RECT* in_extents)
{
    if (using_styles())
    {
        RECT kBigExtents = { 0 };

        kBigExtents.right  = 10000;
        kBigExtents.bottom = 10000;
    
        if (in_extents == 0)
            in_extents = &kBigExtents;

        //
        // Create the font and select it into a temporary device context.
        //
        HDC      tmp_dc = GetDC(0);
        LOGFONTW logical_font = {0};
        HGDIOBJ  font = 0;
        //
        // Try to use get_font, in case there is another font we're meant
        // to use. Then fallback on DEFAULT_GUI_FONT.
        //
        if (get_font(widget_type, logical_font))
            font = (HGDIOBJ)::CreateFontIndirectW(&logical_font);

        assert(font);
        //
        // Extract the extents.
        //
        RECT    tmp_extents = { 0 };
        HGDIOBJ original_font = SelectObject(tmp_dc, font);
        bool    have_extents = S_OK == theme_g->GetThemeTextExtentPtr(theme_g->theme_m,
                                                                      tmp_dc, widget_type,
                                                                      kState, text.c_str(),
                                                                      static_cast<int>(text.size()),
                                                                      DT_CALCRECT + DT_WORDBREAK,
                                                                      in_extents,
                                                                      &tmp_extents);

        if (have_extents)
            out_extents = tmp_extents;
        //
        // Clean up, and convert the size to a rect.
        //
        ::DeleteObject(::SelectObject(tmp_dc, original_font));
        ::ReleaseDC(0, tmp_dc);

        return have_extents;
    }
    else
    {
        //
        // Create the font and select it into a temporary device context.
        //
        LOGFONTW logical_font = { 0 };
        HGDIOBJ  font = 0;
        HDC      tmp_dc = ::GetDC(0);

        //
        // Try to use get_font, in case there is another font we're meant
        // to use. Then fallback on DEFAULT_GUI_FONT.
        //
        if (get_font(widget_type, logical_font))
            font = (HGDIOBJ)::CreateFontIndirectW(&logical_font);

        if (!font)
            font = ::GetStockObject(DEFAULT_GUI_FONT);
        //
        // Extract the extents.
        //
        HGDIOBJ original_font = ::SelectObject(tmp_dc, font);
        SIZE    out_size = { 0 };
        BOOL    have_extents = ::GetTextExtentPoint32W(tmp_dc, text.c_str(), static_cast<int>(text.size()), &out_size);

        //
        // Clean up, and convert the size to a rect.
        //
        ::DeleteObject(::SelectObject(tmp_dc, original_font));
        ::ReleaseDC(0, tmp_dc);

        out_extents.left = 0;
        out_extents.top = 0;
        out_extents.right = out_size.cx;
        out_extents.bottom = out_size.cy;

        return hackery::cast<bool>(have_extents);
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_integer(int widget_type, int measurement, int& out_val)
{
    out_val = 0;

    if (using_styles())
    {
        if (!theme_g->theme_m || widget_type == -1)
            return false;

        return S_OK == theme_g->GetThemeIntPtr(theme_g->theme_m, widget_type, kState, measurement, &out_val);
    }
    else
    {
        if ((widget_type == CP_DROPDOWNBUTTON) && (measurement == TMT_BORDERSIZE) && (target() == L"ComboBox"))
            out_val = 1;
        else if ((widget_type == EP_EDITTEXT) && (measurement == TMT_BORDERSIZE))
            out_val = 1;
        else
            return false;

        return true;
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_size(int widget_type, THEMESIZE measurement, SIZE& out_size)
{
    out_size.cx = 0;
    out_size.cy = 0;

    if (using_styles())
    {
        if (!theme_g->theme_m || widget_type == -1)
            return false;

        HDC     tmp_dc = ::GetDC(0);
        HRESULT hr = theme_g->GetThemePartSizePtr(theme_g->theme_m, tmp_dc, widget_type, kState, 0, measurement, &out_size);

        ::ReleaseDC(0, tmp_dc);

        return (hr == S_OK);
    }
    else
    {
        if (target() == L"Button")
        {
            if ((widget_type == BP_CHECKBOX) || (widget_type == BP_RADIOBUTTON))
            {
                out_size.cx = ::GetSystemMetrics(SM_CXMENUCHECK) + 5 /*gap*/;
                out_size.cy = ::GetSystemMetrics(SM_CYMENUCHECK);
            }
            else if (widget_type == BP_GROUPBOX)
            {
                out_size.cx = 8;
                out_size.cy = 8;
            }
        }
        else if (target() == TRACKBAR_CLASS)
        {
            out_size.cx += ::GetSystemMetrics(SM_CXHTHUMB);
            out_size.cy += ::GetSystemMetrics(SM_CYVTHUMB);
        }
        else
        {
            return false;
        }

        return true;
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_margins(int widget_type, MARGINS& out_margins)
{
    if (using_styles())
    {
        if (!theme_g->theme_m || widget_type == -1)
            return false;

        return S_OK == theme_g->GetThemeMarginsPtr(theme_g->theme_m, 0, widget_type, kState, TMT_CONTENTMARGINS, 0, &out_margins);
    }
    else
    {
        if ((target() == L"Button") && (widget_type == BP_PUSHBUTTON))
        {
            out_margins.cxLeftWidth = 3;
            out_margins.cxRightWidth = 3;
            out_margins.cyTopHeight = 3;
            out_margins.cyBottomHeight = 3;
        }
        else
        {
            return false;
        }

        return true;
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

bool get_button_text_margins(int widget_type, RECT& out_margins)
{
    // in this case, we fall back to the non-theme version if theme is not set.
    if (using_styles() && theme_g->theme_m)
    {
        return S_OK == Button_GetTextMargin(theme_g->window_m, &out_margins);
    }
    else
    {
        if ((target() == L"Button") && (widget_type == BP_CHECKBOX))
        {
            out_margins.top = 1; out_margins.left = 1;
            out_margins.bottom = 1; out_margins.right = 1;
        }
        else
        {
            return false;
        }

        return true;
    }

    // we should never get here, but just in case...
    return false;
}

/****************************************************************************************************/

void draw_parent_background(HWND window, HDC dc)
{
    if (using_styles())
        theme_g->DrawThemeParentBackgroundPtr(window, dc, 0);
}

/****************************************************************************************************/

} // namespace metrics

/****************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(windows_theme_metrics_once, init_windows_theme_metrics_once)

/****************************************************************************************************/

