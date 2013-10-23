/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/windows_graphic_utils.hpp>

#include <adobe/future/windows_cast.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/rgba.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

HBITMAP to_bitmap(const boost::gil::rgba8_image_t& image)
{
    HDC        dst_dc(::GetDC(NULL));
    HDC        src_dc(::CreateCompatibleDC(dst_dc));
    BITMAPINFO bitmap_info = {{ 0 }};
    VOID*      pv_bits;
    HBITMAP    bitmap_handle;
    LONG       image_width(hackery::cast<LONG>(image.width()));
    LONG       image_height(hackery::cast<LONG>(image.height()));

    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = image_width;
    bitmap_info.bmiHeader.biHeight = image_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;         // four 8-bit components
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    bitmap_info.bmiHeader.biSizeImage = image_width * image_height * 4;

    bitmap_handle = ::CreateDIBSection(src_dc, &bitmap_info, DIB_RGB_COLORS, &pv_bits, NULL, 0x0);

    if (bitmap_handle == 0)
        return bitmap_handle;

    boost::gil::bgra8_pixel_t*         dst_bits(static_cast<boost::gil::bgra8_pixel_t*>(pv_bits));
    boost::gil::rgba8_view_t::iterator first(boost::gil::view(const_cast<boost::gil::rgba8_image_t&>(image)).begin());
    boost::gil::rgba8_view_t::iterator last(boost::gil::view(const_cast<boost::gil::rgba8_image_t&>(image)).end());

    std::copy(first, last, dst_bits);

    return bitmap_handle;
}

/****************************************************************************************************/

HCURSOR to_cursor(HBITMAP bitmap, int hotspot_x, int hotspot_y)
{
    static const int max_cursor_width(::GetSystemMetrics(SM_CXCURSOR));
    static const int max_cursor_height(::GetSystemMetrics(SM_CYCURSOR));

    if (bitmap == 0 ||
        hotspot_x < 0 || hotspot_x > max_cursor_width ||
        hotspot_y < 0 || hotspot_y > max_cursor_height)
        return NULL;

    // REVISIT (fbrereto) : Check to see if the bitmap exceeds the max cursor dimensions and,
    //                      if so, scale the bitmap so it can be used successfully as a cursor.

    // This code leans on a CodeGuru article on color cursors:
    // http://www.codeguru.com/cpp/w-p/win32/cursors/article.php/c4529/

    HDC    dc(::GetDC(NULL));
    HDC    main_dc(::CreateCompatibleDC(dc));
    HDC    and_mask_dc(::CreateCompatibleDC(dc));
    HDC    xor_mask_dc(::CreateCompatibleDC(dc));
    BITMAP bm;

    ::GetObject(bitmap, sizeof(bm), &bm);

    HBITMAP and_bitmap(::CreateCompatibleBitmap(dc, bm.bmWidth, bm.bmHeight));
    HBITMAP xor_bitmap(::CreateCompatibleBitmap(dc, bm.bmWidth, bm.bmHeight));

    HBITMAP old_main_bitmap(hackery::cast<HBITMAP>(::SelectObject(main_dc, bitmap)));
    HBITMAP old_and_bitmap(hackery::cast<HBITMAP>(::SelectObject(and_mask_dc, and_bitmap)));
    HBITMAP old_xor_bitmap(hackery::cast<HBITMAP>(::SelectObject(xor_mask_dc, xor_bitmap)));

    boost::uint32_t* src_iter(static_cast<boost::uint32_t*>(bm.bmBits));

    for (int y(0); y < bm.bmHeight; ++y)
    {
        for (int x(0); x < bm.bmWidth; ++x)
        {
            BYTE src_alpha(static_cast<BYTE>((*src_iter) >> 24 & 0xff));

            if (src_alpha == 0)
            {
                ::SetPixel(and_mask_dc, x, y, RGB(255, 255, 255));
                ::SetPixel(xor_mask_dc, x, y, RGB(0, 0, 0));
            }
            else
            {
                BYTE src_r(static_cast<BYTE>((*src_iter) >> 0 & 0xff));
                BYTE src_g(static_cast<BYTE>((*src_iter) >> 8 & 0xff));
                BYTE src_b(static_cast<BYTE>((*src_iter) >> 16 & 0xff));
                BYTE dst_alpha(255 - src_alpha);

                ::SetPixel(and_mask_dc, x, y, RGB(dst_alpha, dst_alpha, dst_alpha));
                ::SetPixel(xor_mask_dc, x, y, RGB(src_r, src_g, src_b));
            }

            ++src_iter;
        }
    }

    ::SelectObject(main_dc, old_main_bitmap);
    ::SelectObject(and_mask_dc, old_and_bitmap);
    ::SelectObject(xor_mask_dc, old_xor_bitmap);

    ::DeleteDC(main_dc);
    ::DeleteDC(and_mask_dc);
    ::DeleteDC(xor_mask_dc);

    ::ReleaseDC(NULL, dc);

    // Now that we have our three bitmaps, we create the cursor

    ICONINFO cursor = { 0 };

    cursor.fIcon = false;
    cursor.xHotspot = hotspot_x;
    cursor.yHotspot = hotspot_y;
    cursor.hbmMask = and_bitmap;
    cursor.hbmColor = xor_bitmap;

    HCURSOR result(::CreateIconIndirect(&cursor));

    ::DeleteObject(and_bitmap);
    ::DeleteObject(xor_bitmap);

    return result;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
