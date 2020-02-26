//
// https://docs.microsoft.com/en-us/windows/win32/winsock/tcp-ip-raw-sockets-2
// https://docs.microsoft.com/en-us/windows/win32/api/_winsock/#functions
//

#pragma once

#include <memory>
#include <vector>

class RawSockets : WSADATA
{
	std::unique_ptr<WSAPROTOCOL_INFO[]> m_protocols;

public:
	RawSockets();
	~RawSockets();

	int GetProtocolCount();
	const wchar_t * GetProtocolName(int index);
};

class RawSocketsMainWindow : RawSockets
{
	SIZE m_offset{};
	SIZE m_client{};

	HWND m_status{};
	HWND m_report{};

public:
	HWND Create(HINSTANCE hInstance, LPCWSTR className, LPCWSTR windowName);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnCreate(HWND hwnd, CREATESTRUCT * createStruct);
	LRESULT OnDestroy(HWND hwnd);
	LRESULT OnSize(HWND hwnd, WINDOWPOS * windowPos);
	LRESULT OnNotify(HWND hwnd, NMHDR * nmhdr);

	enum Mode : UINT { ProtocolView = WM_APP };

	LRESULT ProtocolViewMode();
};

class RawSocketsConfig : Registry
{
public:
	RawSocketsConfig() : Registry(L"Software\\RawSockets") {}
	~RawSocketsConfig() = default;

	void SaveWindowRect(WindowRect &&);
	WindowRect LoadWindowRect();
};
