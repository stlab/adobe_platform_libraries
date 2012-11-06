/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_MACINTOSH_STRING_HPP
#define ADOBE_MACINTOSH_STRING_HPP

/****************************************************************************************************/

#include <adobe/conversion.hpp>
#include <adobe/macintosh_carbon_safe.hpp>
#include <adobe/macintosh_memory.hpp>
#include <adobe/string.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

template<>
inline auto_cfstring_t explicit_cast(const std::string& x)
{
    return auto_cfstring_t(::CFStringCreateWithCString(NULL,
                                                       x.c_str(),
                                                       kCFStringEncodingUTF8));
}

template<>
inline auto_cfstring_t explicit_cast(const adobe::string_t& x)
{
    return auto_cfstring_t(::CFStringCreateWithCString(NULL,
                                                       x.c_str(),
                                                       kCFStringEncodingUTF8));
}

template<>
inline auto_cfstring_t explicit_cast(const adobe::string16_t& x)
{
    return auto_cfstring_t(::CFStringCreateWithCharacters(NULL,
														  x.c_str(),
														  x.size()));
}

/****************************************************************************************************/

template<>
inline std::string explicit_cast(const ::CFStringRef& x)
{
    if (x == 0)
        return std::string();

    const int         pad(16); // must be at least one - seems like a good conservative size
    ::CFIndex         max_size(::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(x),
                                                                   kCFStringEncodingUTF8) + pad);
    std::vector<char> buffer(static_cast<std::size_t>(max_size));

#ifndef NDEBUG
    ::Boolean result =
#endif

    ::CFStringGetCString(x, &buffer[0], max_size, kCFStringEncodingUTF8);

#ifndef NDEBUG
    assert(result);
#endif

    return std::string(&buffer[0]);
}

/****************************************************************************************************/

template<>
inline adobe::string_t explicit_cast(const ::CFStringRef& x)
{
    if (x == 0)
        return adobe::string_t();
	
    const int         pad(16); // must be at least one - seems like a good conservative size
    ::CFIndex         max_size(::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(x),
                                                                   kCFStringEncodingUTF8) + pad);
    std::vector<char> buffer(static_cast<std::size_t>(max_size));
	
#ifndef NDEBUG
    ::Boolean result =
#endif
		
	::CFStringGetCString(x, &buffer[0], max_size, kCFStringEncodingUTF8);
	
#ifndef NDEBUG
    assert(result);
#endif
	
    return adobe::string_t(&buffer[0]);
}

/****************************************************************************************************/

template<>
inline adobe::string16_t explicit_cast(const ::CFStringRef& x)
{
    if (x == 0)
        return adobe::string16_t();
	
    const int         pad(16); // must be at least one - seems like a good conservative size
    ::CFIndex         max_size(::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(x),
                                                                   kCFStringEncodingUTF16) + pad);
    std::vector<boost::uint16_t> buffer(static_cast<std::size_t>(max_size));
	
#ifndef NDEBUG
    ::Boolean result =
#endif
		
	::CFStringGetCString(x, reinterpret_cast<char*>(&buffer[0]), max_size, kCFStringEncodingUTF16);
	
#ifndef NDEBUG
    assert(result);
#endif
	
    return adobe::string16_t(&buffer[0]);
}

/****************************************************************************************************/

template<>
inline std::string explicit_cast(const auto_cfstring_t& x)
{
    return explicit_cast<std::string>(x.get());
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

// ADOBE_MACINTOSH_STRING_HPP
#endif

/****************************************************************************************************/
