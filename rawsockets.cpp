#include "stdafx.h"
#include "logger.h"
#include "rawsockets.h"

using logger::what;

#define FAIL(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }
#define INFO(...) if (::IsDebuggerPresent()) { logger::OutputDebugConsole(MACRO_LOGGER_HEADER(__FILE__, __LINE__, __FUNCTION__), __VA_ARGS__); }


Registry g_config(L"Software\\uemuraj\\RawSockets");


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
		m_statusWindow = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, NULL, hwnd, 100);
		RestoreWindowPos(hwnd);
		return m_rawSockets ? 0 : -1;

	case WM_DESTROY:
		SaveWindowPos(hwnd);
		::PostQuitMessage(0);
		return 0;

	case WM_WINDOWPOSCHANGED:
		if (auto p = (WINDOWPOS *) lParam)
		{
			m_window = *p;
			auto [cx, cy] = GetClientRect(m_window.cx, m_window.cy, ::GetWindowLong(hwnd, GWL_STYLE), ::GetWindowLong(hwnd, GWL_EXSTYLE));
			::SendMessage(m_statusWindow, WM_SIZE, 0, MAKELPARAM(cx, cy));
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RawSocketsMainWindow::RestoreWindowPos(HWND hwnd)
{
	int x = 0, y = 0, cx = 0, cy = 0;

	g_config.Get(L"window.x", x);
	g_config.Get(L"window.y", y);
	g_config.Get(L"window.cx", cx);
	g_config.Get(L"window.cy", cy);

	if (cx > 0 && cy > 0)
	{
		// このウィンドウ位置に最も近いモニタを調べる
		RECT rect{ x, y, x + cx, y + cy };

		MONITORINFO info{ sizeof(info) };
		HMONITOR handle = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

		if (::GetMonitorInfo(handle, &info))
		{
			if (::PtInRect(&info.rcWork, { rect.left, rect.top }) && ::PtInRect(&info.rcWork, { rect.right, rect.bottom }))
			{
				// このモニタに収まるのであれば、ウィンドウの位置とサイズを復元する
				::SetWindowPos(hwnd, HWND_TOP, x, y, cx, cy, SWP_SHOWWINDOW);
			}
			else
			{
				// そうでなければ、ウィンドウのサイズだけを復元する
				::SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW | SWP_NOMOVE);
			}
		}
	}
}

void RawSocketsMainWindow::SaveWindowPos(HWND hwnd)
{
	g_config.Set(L"window.x", m_window.x);
	g_config.Set(L"window.y", m_window.y);
	g_config.Set(L"window.cx", m_window.cx);
	g_config.Set(L"window.cy", m_window.cy);
}

std::pair<int, int> RawSocketsMainWindow::GetClientRect(int cx, int cy, DWORD style, DWORD exStyle, bool menu)
{
	RECT rect{ 0, 0, cx, cy };

	::AdjustWindowRectEx(&rect, style, menu, exStyle);
	
	return { cx + rect.left - (rect.right -cx), cy + rect.top - (rect.bottom -cy) };
}
