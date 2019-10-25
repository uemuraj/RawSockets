//
// https://docs.microsoft.com/en-us/windows/win32/winsock/tcp-ip-raw-sockets-2
// https://docs.microsoft.com/en-us/windows/win32/api/_winsock/#functions
//

#pragma once

#include <memory>
#include <vector>

struct WinSock : WSADATA
{
	WinSock();
	~WinSock();

	std::pair<int, std::unique_ptr<WSAPROTOCOL_INFO[]>> GetProtocols();
};

class RawSockets : WinSock
{
	std::vector<WSAPROTOCOL_INFO> m_protocols;

public:
	RawSockets();
	~RawSockets();

	operator bool()
	{
		return !m_protocols.empty();
	}
};

class RawSocketsMainWindow
{
	RawSockets m_rawSockets;
	WINDOWPOS m_window{};
	HWND m_statusWindow{};

public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void RestoreWindowPos(HWND hwnd);
	void SaveWindowPos(HWND hwnd);

	std::pair<int, int> GetClientRect(int cx, int cy, DWORD style, DWORD exStyle, bool menu = false);
};
