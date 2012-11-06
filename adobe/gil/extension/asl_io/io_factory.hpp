/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_GIL_EXTENSION_IO_FACTORY_HPP
#define ADOBE_GIL_EXTENSION_IO_FACTORY_HPP

/*************************************************************************************************/

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/algorithm.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/poly.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/functional.hpp>

#include <vector>
#include <iostream>
#include <cassert>
#include <stdexcept>

/*************************************************************************************************/

namespace boost { namespace gil {

/*************************************************************************************************/

/*!
    image_io_dispatch is a struct that, by default, will call the procs in your
    class by the same name as they appear in the image_io GIL extension. If your
    class uses different names for detection, reading and writing, you'll want to
    write a specialization of this struct for your class that calls the right
    functions for the operations.
*/

template <typename T, typename ViewType>
struct image_io_dispatch
{
    typedef T                     value_type;
    typedef ViewType              view_type;
    typedef typename ViewType::value_type pixel_type;
    typedef boost::gil::image<pixel_type, 
        boost::gil::is_planar<view_type>::value> image_type;

    inline bool detect(const value_type& value,
                       std::streambuf&   stream_buffer) const
        { return value.detect(stream_buffer); }

    inline void read(const value_type&    value,
                     image_type&          image,
                     std::streambuf&      stream_buffer,
                     adobe::dictionary_t& parameters) const
        { value.read(image, stream_buffer, parameters); }
 
    inline void write(const value_type&   value,
                      const view_type&    image_view,
                      std::streambuf&     stream_buffer,
                      adobe::dictionary_t parameters = adobe::dictionary_t()) const
        { value.write(image_view, stream_buffer, parameters); }
};

/*************************************************************************************************/

template <typename ViewType>
class image_factory_t;

/*************************************************************************************************/

/*!
    image_format_t is the poly_object wrapper for
    the file format io protocol to a given format
*/

template <typename ViewType>
class image_format_t
{
    typedef ViewType              view_type;
    typedef typename view_type::value_type        pixel_type;
    typedef boost::gil::image<pixel_type,
         boost::gil::is_planar<view_type>::value> image_type;

    struct protocol : adobe::poly_copyable_interface
    {
        virtual bool detect(std::streambuf& stream_buffer) const = 0;

        virtual void read(image_type&          image,
                          std::streambuf&      stream_buffer,
                          adobe::dictionary_t& parameters) const = 0;

       virtual void write(const view_type&    image_view,
                          std::streambuf&     stream_buffer,
                          adobe::dictionary_t parameters = adobe::dictionary_t()) const = 0;
    };


    template <typename T> // T models ImageFileIOType
    struct instance : adobe::optimized_storage_type<T, protocol>::type {
        typedef typename adobe::optimized_storage_type<T, protocol>::type base_t;

        typedef T value_type;

        instance(const T& x) : base_t(x) { }
        instance(adobe::move_from<instance> x) 
            : base_t(adobe::move_from<base_t>(x.source)){ }

        bool detect(std::streambuf& stream_buffer) const
            { return image_io_dispatch<T, view_type>().detect(this->get(), stream_buffer); }

        void read(image_type&          image,
                  std::streambuf&      stream_buffer,
                  adobe::dictionary_t& parameters) const
            { image_io_dispatch<T, view_type>().read(this->get(), image, stream_buffer, parameters); }

        void write(const view_type&    image_view,
                   std::streambuf&     stream_buffer,
                   adobe::dictionary_t parameters = adobe::dictionary_t()) const
            { image_io_dispatch<T, view_type>().write(this->get(), image_view, stream_buffer, parameters); }
    };

public:
    template <typename T> // T models ImageFileIOType
    image_format_t(adobe::name_t tag, const T& f) :
        tag_m(tag),
        object_m(f)
    { }

    bool detect(std::streambuf& stream_buffer) const
        { return object_m->detect(stream_buffer); }

    adobe::name_t read(image_type&          image,
                       std::streambuf&      stream_buffer,
                       adobe::dictionary_t& parameters) const
        { object_m->read(image, stream_buffer, parameters); return tag(); }

    void write(const view_type&    image_view,
               std::streambuf&     stream_buffer,
               adobe::dictionary_t parameters = adobe::dictionary_t()) const
        { object_m->write(image_view, stream_buffer, parameters); }

    adobe::name_t tag() const
        { return tag_m; }

    friend inline bool operator==(const image_format_t& x, const image_format_t& y)
    { return x.tag_m == y.tag_m; }

private:
    adobe::name_t                               tag_m;
    adobe::poly_base<protocol, instance>        object_m;

    friend class image_factory_t<view_type>;
};

/*************************************************************************************************/

template <typename ViewType>
class image_factory_t
{
public:
    typedef ViewType                              view_type;
    typedef typename view_type::value_type        pixel_type;
    typedef boost::gil::image<pixel_type,
         boost::gil::is_planar<view_type>::value> image_type;
    typedef image_format_t<view_type>             image_format_type;
    typedef std::vector<image_format_type>        format_set_t;
    typedef typename format_set_t::value_type     value_type;
    typedef typename format_set_t::iterator       iterator;
    typedef typename format_set_t::const_iterator const_iterator;

    template <typename O>
    O detect_all_for(std::streambuf& stream_buffer,
                     O               output) const
    {
        for (const_iterator first(format_set_m.begin()), last(format_set_m.end());
                first != last; ++first)
        {
            if (first->detect(stream_buffer))
                *output++ = first->tag();
        }

        return output;
    }

    adobe::name_t read(image_type&          image,
                       std::streambuf&      stream_buffer,
                       adobe::dictionary_t& parameters,
                       adobe::name_t        format_tag = adobe::name_t())
    {
        return format_tag == adobe::name_t() ?
            detect_first_for(stream_buffer).read(image, stream_buffer, parameters) :
            find(format_tag)->read(image, stream_buffer, parameters);
    }

    void write(const view_type&     image_view,
               std::streambuf&      stream_buffer,
               adobe::name_t        format_tag,
               adobe::dictionary_t  parameters = adobe::dictionary_t())
    {
        if (format_tag == adobe::name_t())
            throw std::runtime_error("gil: image_io: format tag required for export");

        find(format_tag)->write(image_view, stream_buffer, parameters);
    }

    void register_format(const image_format_type& format)
    {
        if (format.tag() == adobe::name_t())
            throw std::runtime_error("gil: image_io: Illegal tag name");

        format_set_m.push_back(format);
    }

    bool is_registered(adobe::name_t format_tag)
    {
        if (format_tag == adobe::name_t())
            throw std::runtime_error("gil: image_io: Illegal tag name");

        try
        {
            find(format_tag);
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

    void unregister_format(adobe::name_t format_tag)
    {
        if (format_tag == adobe::name_t())
            throw std::runtime_error("GIL IO Factory: Illegal tag name");

        format_set_m.erase(find(format_tag));
    }

private:
    inline iterator find(adobe::name_t format_tag)
    {
        iterator result(adobe::find_if(format_set_m,
                                       boost::bind(adobe::compare_members(&value_type::tag_m,
                                                                          std::equal_to<adobe::name_t>()),
                                                   format_tag,
                                                   _1)));

        if (result == format_set_m.end())
            throw std::runtime_error("gil: image_io: format not found");

        return result;
    }

    value_type& detect_first_for(std::streambuf& stream_buffer)
    {
        iterator result(adobe::find_if(format_set_m,
                                       boost::bind(&value_type::detect,
                                                   _1,
                                                   boost::ref(stream_buffer))));

        if (result == format_set_m.end())
            throw std::runtime_error("gil: image_io: format not detected");

        return *result;
    }

    format_set_t format_set_m;
};

/*************************************************************************************************/

} } // namespace boost::gil

/*************************************************************************************************/

#endif

/*************************************************************************************************/
