#pragma once

#include <utility>
#include <sstream>

#define MACRO_LOGGER_NUMBER2TEXT2(n)	#n
#define MACRO_LOGGER_NUMBER2TEXT(n)	MACRO_LOGGER_NUMBER2TEXT2(n)
#define MACRO_LOGGER_HEADER(file, line, func)	TEXT(file "(" MACRO_LOGGER_NUMBER2TEXT(line) "): " func)

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
		const wchar_t * message;

	public:
		what(long c) : code(c), message(nullptr) {}
		~what();

		std::wostream & Print(std::wostream & os) const;
	};

	inline std::wostream & operator<<(std::wostream & os, const what & error)
	{
		return error.Print(os);
	}
}
