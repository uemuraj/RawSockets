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
		return OnCreate(hwnd, (CREATESTRUCT *) lParam);

	case WM_DESTROY:
		return OnDestroy(hwnd);

	case WM_WINDOWPOSCHANGED:
		if ((((WINDOWPOS *) lParam)->flags & SWP_NOSIZE) == 0)
		{
			return OnSize(hwnd, (WINDOWPOS *) lParam);
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT RawSocketsMainWindow::OnCreate(HWND hwnd, CREATESTRUCT * createStruct)
{
	m_offset = ::GetClientRectSizeOffset(createStruct);
	m_client = ::GetClientRectSize(createStruct, m_offset);
	m_status = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, nullptr, hwnd, 100);
	m_report = ::CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER, 0, 0, m_client.cx, m_client.cy - WindowPos(m_status).cy, hwnd, (HMENU) 200, createStruct->hInstance, nullptr);

	// TODO: ステータスバーの高さを保存しておく

	// TODO: リストビューのスタイルを決める
	wchar_t szColmun[] = L"カラム０";
	wchar_t szItem[] = L"アイテム０";

	LVCOLUMN lvc{ LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM };

	lvc.iSubItem = 0;
	lvc.pszText = szColmun;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;

	LVITEM lvi{ LVIF_TEXT };
	lvi.pszText = szItem;

	ListView_InsertColumn(m_report, 0, &lvc);
	ListView_InsertItem(m_report, &lvi);
	//ListView_SetExtendedListViewStyleEx(m_report, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);

	return 0;
}

LRESULT RawSocketsMainWindow::OnDestroy(HWND hwnd)
{
	g_config.SaveWindowPos(hwnd);
	::PostQuitMessage(0);
	return 0;
}

LRESULT RawSocketsMainWindow::OnSize(HWND hwnd, WINDOWPOS * windowPos)
{
	m_client = ::GetClientRectSize(windowPos, m_offset);
	::SendMessage(m_status, WM_SIZE, 0, MAKELPARAM(m_client.cx, m_client.cy));
	::SetWindowPos(m_report, nullptr, 0, 0, m_client.cx, m_client.cy - WindowPos(m_status).cy, SWP_NOMOVE);
	return 0;
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
	int x = 0, y = 0, cx = 0, cy = 0;

	Get(L"window.x", x);
	Get(L"window.y", y);
	Get(L"window.cx", cx);
	Get(L"window.cy", cy);

	if (x > 0 && y > 0 && cx > 0 && cy > 0)
	{
		// 最も近いモニタを調べ、ウィンドウ全体が表示可能であれば、位置とサイズを採用する
		RECT rect = WindowPos(x, y, cx, cy);
		HMONITOR handle = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info{ sizeof(info) };

		if (::GetMonitorInfo(handle, &info))
		{
			if (::PtInRect(&info.rcWork, { rect.left, rect.top }) && ::PtInRect(&info.rcWork, { rect.right, rect.bottom }))
			{
				return { x, y, cx, cy };
			}

			return { CW_USEDEFAULT, 0, cx, cy }; // 位置は採用しない
		}
	}

	return { CW_USEDEFAULT, 0, CW_USEDEFAULT, 0 }; // 位置もサイズも採用しない
}
