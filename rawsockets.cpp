#include "stdafx.h"
#include "logger.h"
#include "rawsockets.h"

using logger::what;

RawSocketsConfig g_config(L"Software\\uemuraj\\RawSockets");


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
		INFO(what(error));

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

HWND RawSocketsMainWindow::Create(HINSTANCE hInstance, LPCWSTR className, LPCWSTR windowName)
{
	auto [x, y, cx, cy] = g_config.LoadWindowPos();

	return ::CreateWindowEx(0, className, windowName, WS_OVERLAPPEDWINDOW, x, y, cx, cy, nullptr, nullptr, hInstance, nullptr);
}

LRESULT RawSocketsMainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		m_offset = ::GetClientRectSizeOffset((CREATESTRUCT *) lParam);
		m_client = ::GetClientRectSize((CREATESTRUCT *) lParam, m_offset);
		m_status = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, nullptr, hwnd, 100);
		return m_rawSockets ? 0 : -1;

	case WM_DESTROY:
		g_config.SaveWindowPos(hwnd);
		::PostQuitMessage(0);
		return 0;

	case WM_WINDOWPOSCHANGED:
		if ((((WINDOWPOS *) lParam)->flags & SWP_NOSIZE) == 0)
		{
			m_client = ::GetClientRectSize((WINDOWPOS *) lParam, m_offset);
			::SendMessage(m_status, WM_SIZE, 0, MAKELPARAM(m_client.cx, m_client.cy));
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RawSocketsConfig::SaveWindowPos(WindowPos && windowPos)
{
	Set(L"window.x", windowPos.x);
	Set(L"window.y", windowPos.y);
	Set(L"window.cx", windowPos.cx);
	Set(L"window.cy", windowPos.cy);
}

WindowPos RawSocketsConfig::LoadWindowPos()
{
	WindowPos windowPos(CW_USEDEFAULT, 0, CW_USEDEFAULT, 0);

	Get(L"window.x", windowPos.x);
	Get(L"window.y", windowPos.y);
	Get(L"window.cx", windowPos.cx);
	Get(L"window.cy", windowPos.cy);

	if (windowPos.x > 0 && windowPos.y > 0 && windowPos.cx > 0 && windowPos.cy > 0)
	{
		// 最も近いモニタを調べ、ウィンドウ全体が表示可能であれば、位置とサイズを採用する
		RECT rect = windowPos;
		HMONITOR handle = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info{ sizeof(info) };

		if (::GetMonitorInfo(handle, &info))
		{
			if (::PtInRect(&info.rcWork, { rect.left, rect.top }) && ::PtInRect(&info.rcWork, { rect.right, rect.bottom }))
			{
				return windowPos;
			}

			windowPos.x = CW_USEDEFAULT; // 位置は採用しない
			return windowPos;
		}
	}

	windowPos.x = windowPos.cx = CW_USEDEFAULT; // 位置もサイズも採用しない
	return windowPos;
}
