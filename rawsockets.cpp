#include "stdafx.h"
#include "logger.h"
#include "rawsockets.h"

using logger::what;

#define FAIL(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }
#define INFO(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }


WinSock::WinSock() : WSADATA{}
{
	if (auto error = ::WSAStartup(MAKEWORD(2, 2), this))
	{
		FAIL(what(error));
	}
	else
	{
		INFO(HIBYTE(wHighVersion), LOBYTE(wHighVersion));
	}
}

WinSock::~WinSock()
{
	if (::WSACleanup())
	{
		FAIL(what(::WSAGetLastError()));
	}
}

std::pair<int, std::unique_ptr<WSAPROTOCOL_INFO[]>> WinSock::GetProtocols()
{
	int error{};
	DWORD length{};

	if (::WSCEnumProtocols(nullptr, nullptr, &length, &error) == SOCKET_ERROR)
	{
		if (error == WSAENOBUFS)
		{
			size_t n = length / sizeof(WSAPROTOCOL_INFO) + 1; // バイト数でなく要素数が必要

			auto buffer = std::make_unique<WSAPROTOCOL_INFO[]>(n);
			auto count = ::WSCEnumProtocols(nullptr, buffer.get(), &length, &error);

			if (count != SOCKET_ERROR)
			{
				return { count, std::move(buffer) };
			}
		}
	}

	FAIL(what(error));
	return {};
}

RawSockets::RawSockets()
{
	auto [count, protocols] = GetProtocols();

	for (decltype(count) i = 0; i < count; ++i)
	{
		INFO(protocols[i].iSocketType, protocols[i].szProtocol);

		if (protocols[i].iSocketType == SOCK_RAW)
		{
			m_protocols.emplace_back(protocols[i]);
		}
	}
}

RawSockets::~RawSockets()
{
}

LRESULT RawSocketsMainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return m_rawSockets ? 0 : -1;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
