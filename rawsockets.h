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
	SIZE m_offset{};
	SIZE m_client{};
	HWND m_status{};

	RawSockets m_rawSockets;

public:
	HWND Create(HINSTANCE hInstance, LPCWSTR className, LPCWSTR windowName);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

struct RawSocketsConfig : Registry
{
	using Registry::Registry;

	void SaveWindowPos(WindowPos && windowPos);
	WindowPos LoadWindowPos();
};
