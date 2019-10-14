#include "stdafx.h"
#include "rawsockets.h"

// https://docs.microsoft.com/en-us/windows/win32/winsock/tcp-ip-raw-sockets-2
// https://docs.microsoft.com/en-us/windows/win32/api/_winsock/#functions

WinSock::WinSock() : WSADATA{}
{
	m_error = ::WSAStartup(MAKEWORD(2, 2), this);

	if (!m_error)
	{
		INFO(HIBYTE(wHighVersion), LOBYTE(wHighVersion));
	}
	else
	{
		FAIL(what(m_error));
	}
}

WinSock::~WinSock()
{
	::WSACleanup();
}

std::pair<int, std::unique_ptr<WSAPROTOCOL_INFO[]>>  WinSock::GetProtocols()
{
	DWORD length{};

	if (::WSCEnumProtocols(nullptr, nullptr, &length, &m_error) == SOCKET_ERROR)
	{
		if (m_error == WSAENOBUFS)
		{
			size_t n = length / sizeof(WSAPROTOCOL_INFO) + 1; // バイト数でなく要素数が必要

			auto buffer = std::make_unique<WSAPROTOCOL_INFO[]>(n);
			auto count = ::WSCEnumProtocols(nullptr, buffer.get(), &length, &m_error);

			if (count != SOCKET_ERROR)
			{
				return { count, std::move(buffer) };
			}
		}
	}

	FAIL(what(m_error));
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
