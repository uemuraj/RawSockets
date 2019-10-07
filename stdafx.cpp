#include "stdafx.h"

// Link with ws2_32.lib and ole32.lib
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "ole32.lib")

logger::what::~what()
{
	::LocalFree((HLOCAL) message);
}

std::wostream & logger::what::Print(std::wostream & os) const
{
	os << code;

	if (message == nullptr)
	{
		DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;

		if (::FormatMessage(flags, nullptr, code, 0, (LPTSTR)&message, 0, nullptr) == 0)
		{
			return os;
		}
	}

	return os << L' ' << message;
}
