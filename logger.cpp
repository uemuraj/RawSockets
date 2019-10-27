#include "stdafx.h"
#include "logger.h"

namespace logger
{
	std::wostream & what::Print(std::wostream & os) const
	{
		wchar_t * message{};

		DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;

		if (::FormatMessage(flags, nullptr, code, 0, (LPWSTR) &message, 0, nullptr) == 0)
		{
			return os << code;
		}

		os << code << L' ' << message;

		::HeapFree(::GetProcessHeap(), 0, message);

		return os;
	}
}
