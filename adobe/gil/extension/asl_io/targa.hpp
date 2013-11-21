/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_GIL_EXTENSION_IMAGE_IO_TARGA_HPP
#define ADOBE_GIL_EXTENSION_IMAGE_IO_TARGA_HPP

/*************************************************************************************************/

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/future/endian.hpp>

#include <boost/cstdint.hpp>

#include <stdexcept>

/*************************************************************************************************/

namespace boost { namespace gil {

/*************************************************************************************************/

template <>
struct default_color_converter_impl<rgb_t, rgba_t> {
    template <typename P1, typename P2> 
    void operator()(const P1& src, P2& dst) {
        get_color(dst,red_t())=get_color(src,red_t());
        get_color(dst,green_t())=get_color(src,green_t());
        get_color(dst,blue_t())=get_color(src,blue_t());
        get_color(dst,alpha_t())=-1;
    }
};

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

struct targa_header_t
{
    boost::uint8_t  id_length_m;
    boost::uint8_t  color_map_type_m;
    boost::uint8_t  image_type_m;
    boost::uint16_t color_map_start_m;
    boost::uint16_t color_map_length_m;
    boost::uint8_t  color_map_depth_m;
    boost::uint16_t x_offset_m;
    boost::uint16_t y_offset_m;
    boost::uint16_t width_m;
    boost::uint16_t height_m;
    boost::uint8_t  pixel_depth_m;
    boost::uint8_t  image_descriptor_m;
};

/*************************************************************************************************/

enum targa_image_data_type
{
    data_type_none_s            = 0,
    data_type_colormapped_s     = 1,
    data_type_truecolor_s       = 2,
    data_type_monochrome_s      = 3,
    data_type_colormapped_rle_s = 9,
    data_type_truecolor_rle_s   = 10,
    data_type_monochrome_rle_s  = 11
};

/*************************************************************************************************/

template <adobe::endian::type StreamEndian>
class streambuf_swizzler_t
{
public:
    typedef void result_type;

    explicit streambuf_swizzler_t(std::streambuf& buffer) :
        buf_m(buffer)
    { }

    template <typename T>
    result_type read(T& x)
    {
        enum { size = sizeof(T) };

        if (buf_m.sgetn(reinterpret_cast<char*>(&x), size) != size)
            throw std::runtime_error("streambuf_swizzler_t read error");

        adobe::endian_swap<StreamEndian>()(x);
    }

    template <typename T>
    inline result_type operator()(T& x)
    { read(x); }

    template <typename T>
    inline result_type operator()(T* first, T* last)
    {
        for (; first != last; ++first)
            (*this)(*first);
    }

private:
    std::streambuf& buf_m;
};

/****************************************************************************************************/

template<>
template<>
inline void streambuf_swizzler_t<adobe::endian::little>::operator()<>(boost::gil::argb8_pixel_t& x)
{
    read(get_color(x,blue_t()));
    read(get_color(x,green_t()));
    read(get_color(x,red_t()));
    read(get_color(x,alpha_t()));
}

/****************************************************************************************************/

template<>
template<>
inline void streambuf_swizzler_t<adobe::endian::little>::operator()<>(boost::gil::rgb8_pixel_t& x)
{
    read(get_color(x,blue_t()));
    read(get_color(x,green_t()));
    read(get_color(x,red_t()));
}

/****************************************************************************************************/

static void targa_read_header(std::streambuf& stream_buffer, targa_header_t& dst)
{
    streambuf_swizzler_t<adobe::endian::little> reader(stream_buffer);

    // We read in the header chunks one contiguous type set at
    // a time to work around padding that may be introduced between
    // the different types to achieve word alignment.

    //reader(dst.id_length_m);
    //reader(dst.color_map_type_m);
    //reader(dst.image_type_m);
    reader(&dst.id_length_m, &dst.image_type_m + 1);

    //reader(dst.color_map_start_m);
    //reader(dst.color_map_length_m);
    reader(&dst.color_map_start_m, &dst.color_map_length_m + 1);

    reader(dst.color_map_depth_m);

    //reader(dst.x_offset_m);
    //reader(dst.y_offset_m);
    //reader(dst.width_m);
    //reader(dst.height_m);
    reader(&dst.x_offset_m, &dst.height_m + 1);

    //reader(dst.pixel_depth_m);
    //reader(dst.image_descriptor_m);
    reader(&dst.pixel_depth_m, &dst.image_descriptor_m + 1);
}

/*************************************************************************************************/

template <typename Image, adobe::endian::type SourceEndian>
void read_rle_pixels(Image&         img,
                     streambuf_swizzler_t<SourceEndian>& stream)
{
    typedef typename Image::value_type     value_type;
    typedef typename Image::view_t         view_type;
    typedef typename view_type::iterator   iterator;

	unsigned char                    rle_header;
	value_type                       rle_color;
    view_type img_view = boost::gil::view(img);
    iterator                         first(img_view.begin());
    iterator                         last(img_view.end());

    // NOTE (fbrereto) : We don't do the color conversion here; 
    //                   we save that for stuff_image, who can
    //                   do it for both rle and non-rle cases.

	while (first != last)
	{
        std::size_t num_pixels(0);

		stream(rle_header);

		if (rle_header & char(0x80)) // rle compressed data flag check
		{
		    num_pixels = (rle_header & 0x7f) + 1;

			stream(rle_color);

			for (std::size_t i(0); i < num_pixels; ++i)
			{
                *first = rle_color;

                ++first;
			}
		}
		else // raw data
		{
			num_pixels = (rle_header + 1);

            std::vector<value_type> slurp(num_pixels);

			stream(&slurp[0], &slurp[0] + slurp.size());

            first = std::copy(&slurp[0], &slurp[0] + slurp.size(), first);
		}
	}
}

/*************************************************************************************************/

template <typename SourcePixelType, typename Image, adobe::endian::type SourceEndian>
void stuff_image(Image&                              img,
                 streambuf_swizzler_t<SourceEndian>& stream,
                 bool                                rle_compression)
{
    typedef boost::gil::image<SourcePixelType,false> src_image;
    typedef typename src_image::view_t               src_view_type;
    typedef typename src_view_type::iterator         src_iterator;
    typedef typename Image::value_type               dst_value_type;

    src_image src(img.dimensions());
    src_view_type src_view(boost::gil::view(src));

    if (rle_compression)
        read_rle_pixels(src, stream);
    else
    {
        // REVISIT (fbrereto) : boost::bind me

        src_iterator src_first(src_view.begin());
        src_iterator src_last(src_view.end());

        for (; src_first != src_last; ++src_first)
            stream(*src_first);
    }

    std::transform(src_view.begin(), src_view.end(), boost::gil::view(img).begin(), boost::gil::color_convert_deref_fn<SourcePixelType,dst_value_type>());
}

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

template <typename ViewType>
class targa_t
{
private:
    typedef ViewType                                                 view_type;
    typedef typename ViewType::value_type                            pixel_type;
    typedef boost::gil::image<pixel_type,is_planar<ViewType>::value> image_type;

public:
    bool detect(std::streambuf& stream_buffer) const;

    void read(image_type&          img,
              std::streambuf&      stream_buffer,
              adobe::dictionary_t& parameters) const;

    void write(const view_type&    img_view,
               std::streambuf&     stream_buffer,
               adobe::dictionary_t parameters = adobe::dictionary_t()) const;

    friend inline bool operator==(const targa_t&, const targa_t&)
        { return true; }
};

/*************************************************************************************************/

template <typename ConceptSpace> // REVISIT (fbrereto) : Maybe needs to be a ConceptSpaceView?
class targa_variant_t
{
public:
    bool detect(std::streambuf& stream_buffer) const
        { return targa_t<boost::gil::argb8_view_t>().detect(stream_buffer); }

    template <typename C>
    void read(boost::gil::any_image<C>&     img,
              std::streambuf&      stream_buffer,
              adobe::dictionary_t& parameters) const
        { img = boost::gil::argb8_image_t(); targa_t<boost::gil::argb8_view_t>().read(img, stream_buffer, parameters); }

    template <typename CV>
    void write(const boost::gil::any_image_view<CV>& /*img_view*/,
               std::streambuf&         /*stream_buffer*/,
               adobe::dictionary_t     /*parameters = adobe::dictionary_t()*/) const
        { throw std::runtime_error("gil: image_io: targa: specific format not supported"); }
};

/*************************************************************************************************/

template <typename ViewType>
bool targa_t<ViewType>::detect(std::streambuf& stream_buffer) const
{
    implementation::targa_header_t header;

    try
    {
        stream_buffer.pubseekpos(0);

        implementation::targa_read_header(stream_buffer, header);
    }
    catch (...)
    {
        return false;
    }

    return (header.image_type_m == implementation::data_type_truecolor_s ||
               header.image_type_m == implementation::data_type_truecolor_rle_s) &&
           (header.pixel_depth_m == 24 || header.pixel_depth_m == 32);
}

/*************************************************************************************************/

template <typename ViewType>
void targa_t<ViewType>::read(image_type&          img,
                             std::streambuf&      stream_buffer,
                             adobe::dictionary_t& /*parameters*/) const
{
    stream_buffer.pubseekpos(0);

    implementation::targa_header_t header;

    implementation::targa_read_header(stream_buffer, header);

    bool rle_compression(header.image_type_m == implementation::data_type_truecolor_rle_s);

    if (header.id_length_m)
        stream_buffer.pubseekpos(sizeof(header) + header.id_length_m);

    implementation::streambuf_swizzler_t<adobe::endian::little> stream(stream_buffer);

    img.recreate(header.width_m, header.height_m);

    if (header.pixel_depth_m == 24)
        implementation::stuff_image<boost::gil::rgb8_pixel_t>(img, stream, rle_compression);
    else if (header.pixel_depth_m == 32)
        implementation::stuff_image<boost::gil::argb8_pixel_t>(img, stream, rle_compression);
    else
        throw std::runtime_error("gil: image_io: targa: specific format not supported");

    /*
        The image header tells us where the origin point is for the image. The values are bits 4 and
        5 of the image_descriptor_m variable. By default the values are (0,0), meaning bottom-left.
        If the bits are not set to bottom-left, then we have some axis-flipping of the image to do in
        order to get its orientation correct.
    */

    bool origin_at_bottom((header.image_descriptor_m >> 5 & 0x01) == 0);
    bool origin_at_left((header.image_descriptor_m >> 4 & 0x01) == 0);

    // is x origin on the right? If so, flip the image horizontally.
    if (!origin_at_left) 
    {
        image_type tmp;

        tmp.recreate(header.width_m, header.height_m);

        copy_pixels(flipped_left_right_view(view(img)), view(tmp));

        img = tmp;
    }

    // is y origin on the bottom? If so, flip the image vertically.
    if (!origin_at_bottom)
    {
        image_type tmp;

        tmp.recreate(header.width_m, header.height_m);

        copy_pixels(flipped_up_down_view(view(img)), view(tmp));

        img = tmp;
    }
}

/*************************************************************************************************/

template <typename ViewType>
void targa_t<ViewType>::write(const view_type&    /*img_view*/,
                              std::streambuf&     /*stream_buffer*/,
                              adobe::dictionary_t /*parameters*/) const
{
    throw std::runtime_error("gil: image_io: targa: writing not supported");
}

/*************************************************************************************************/

} } // namespace boost::gil

/*************************************************************************************************/

#endif

/*************************************************************************************************/
