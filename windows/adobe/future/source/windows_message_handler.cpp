/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/future/windows_message_handler.hpp>

#include <stdexcept>

/**************************************************************************************************/

namespace adobe {
	
/**************************************************************************************************/
	
message_handler_t::message_handler_t(callback_t callback) :
	property_m(),
	callback_m(callback)
{ }

/**************************************************************************************************/

message_handler_t::~message_handler_t()
{
	uninstall();
}

/**************************************************************************************************/

void message_handler_t::uninstall()
{
	if (!is_installed())
		return;

	property_m->handler_m = NULL;
	property_m = NULL;
}

/**************************************************************************************************/

void message_handler_t::install_implementation(HWND window, const char* proc_name, WNDPROC win_proc)
{
	if (is_installed())
		throw std::runtime_error("Message handler already bound to a target.");
	
	if (NULL != get_handler_property(window, proc_name))
		throw std::runtime_error(std::string("Handler already installed with name ") + proc_name);
	
	property_m = new property_t;
	property_m->handler_m = this;

#if defined(_WIN64)
	typedef LONG_PTR	windows_long_t;
#else
	typedef LONG		windows_long_t;
#endif

#if ADOBE_PLATFORM_WIN
// because it might be cygwin (eberdahl) 
#pragma warning(push)
#pragma warning(disable: 4311) // suppress warning for casting WNDPROC to LONG
#pragma warning(disable: 4312) // suppress warning for casting LONG to WNDPROC
#endif	
	property_m->next_proc_m = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<windows_long_t>(win_proc)));

#if ADOBE_PLATFORM_WIN
// because it might be cygwin (eberdahl)
#pragma warning(pop)
#endif	
	set_handler_property(window, proc_name, property_m);
}

/**************************************************************************************************/

LRESULT message_handler_t::win_proc_implementation(HWND			window,
												   UINT			message,
												   WPARAM		wparam,
												   LPARAM		lparam,
												   const char*	proc_name)
{
	LRESULT result(0);
	
	property_t* const property = get_handler_property(window, proc_name);
	
	try
	{
		if (NULL != property && NULL != property->handler_m && !property->handler_m->callback_m.empty())
			result = property->handler_m->callback_m(window, message, wparam, lparam, property->next_proc_m);
		else
			result = CallWindowProc(property->next_proc_m, window, message, wparam, lparam);
	}
	catch (...)
	{
		// this space intentionally left blank
	}
	
	try
	{
		// Cleaning up the handler property gets its own try-catch block to isolate the case where
		// the handler callback might throw an exception. Even in that situation, we need to clean
		// up the handler property when the HWND is destroyed.
		if (WM_NCDESTROY == message)
		{
			if (property->handler_m != NULL)
			{
				property->handler_m->property_m = NULL;
			}
			
			remove_handler_property(window, proc_name);
			delete property;
		}
	}
	catch (...)
	{
		// this space intentionally left blank
	}
	
	return result;
}

/**************************************************************************************************/

message_handler_t::property_t* message_handler_t::get_handler_property(HWND window, const char* prop_name)
{
	return reinterpret_cast<property_t*>(::GetPropA(window, prop_name));
}

/**************************************************************************************************/

void message_handler_t::set_handler_property(HWND window, const char* prop_name, property_t* property)
{
	::SetPropA(window, prop_name, property);
}

/**************************************************************************************************/

void message_handler_t::remove_handler_property(HWND window, const char* prop_name)
{
	::RemovePropA(window, prop_name);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
