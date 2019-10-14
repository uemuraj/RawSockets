#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#include <ws2spi.h>

#if !defined(UNICODE)
#error
#endif

#include <memory>
#include <utility>
#include <sstream>
#include <vector>

namespace logger
{
	class what
	{
		const HRESULT code;
		const wchar_t * message;

	public:
		what(HRESULT hr) : code(hr), message(nullptr) {}
		~what();

		std::wostream & Print(std::wostream & os) const;
	};

	inline std::wostream & operator<<(std::wostream & os, const what & error)
	{
		return error.Print(os);
	}

	template <typename P>
	std::wstringstream & message(std::wstringstream && buf, P && value)
	{
		buf << L' ' << value << L"\r\n";

		return buf;
	}

	template <typename P, typename ... Q>
	std::wstringstream & message(std::wstringstream && buf, P && value, Q && ... rest)
	{
		buf << L' ' << value;

		return message(std::move(buf), std::move(rest)...);
	}
}

#define MACRO_LOGGER_NUMBER2TEXT2(n)	#n
#define MACRO_LOGGER_NUMBER2TEXT(n)	MACRO_LOGGER_NUMBER2TEXT2(n)
#define MACRO_LOGGER_HEADER(file, line, func)	TEXT(file "(" MACRO_LOGGER_NUMBER2TEXT(line) "): " func)

#define FAIL(...)	if (::IsDebuggerPresent()) { ::OutputDebugString(logger::message(std::wstringstream(), MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__).str().c_str()); }
#define INFO(...)	if (::IsDebuggerPresent()) { ::OutputDebugString(logger::message(std::wstringstream(), MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__).str().c_str()); }

using logger::what;
