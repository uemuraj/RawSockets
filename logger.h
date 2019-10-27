#pragma once

#include <utility>
#include <sstream>

#define MACRO_LOGGER_NUMBER2TEXT2(n)	#n
#define MACRO_LOGGER_NUMBER2TEXT(n)	MACRO_LOGGER_NUMBER2TEXT2(n)
#define MACRO_LOGGER_HEADER(file, line, func)	TEXT(file "(" MACRO_LOGGER_NUMBER2TEXT(line) "): " func)

#define FAIL(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }
#define INFO(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }

namespace logger
{
	template <typename P>
	std::wostream & message(std::wostream & os, P && value)
	{
		return os << L' ' << value;
	}

	template <typename P, typename ... Q>
	std::wostream & message(std::wostream & os, P && value, Q && ... rest)
	{
		return message(os << L' ' << value, std::forward<Q>(rest)...);
	}

	template <typename ... P>
	void OutputDebugConsole(P && ... values)
	{
		std::wstringstream buf;

		message(buf, std::forward<P>(values)...) << L"\r\n";

		::OutputDebugString(buf.str().c_str());
	}

	class what
	{
		const long code;

	public:
		what(long c) : code(c) {}
		~what() = default;

		std::wostream & Print(std::wostream & os) const;
	};

	inline std::wostream & operator<<(std::wostream & os, const what & error)
	{
		return error.Print(os);
	}
}
