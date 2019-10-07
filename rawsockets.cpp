#include "stdafx.h"
#include "rawsockets.h"

// https://docs.microsoft.com/en-us/windows/win32/winsock/tcp-ip-raw-sockets-2
// https://docs.microsoft.com/en-us/windows/win32/api/_winsock/#functions

LRESULT RawSocketsMainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		CheckRawSocketSupport();
		return 0;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

bool RawSocketsMainWindow::CheckRawSocketSupport()
{
	WinSock winSock;

	DWORD bufferLength = sizeof(WSAPROTOCOL_INFO) * 60;
	auto protocolInfo = std::make_unique<WSAPROTOCOL_INFO[]>(60);

	int error{};
	int count = ::WSCEnumProtocols(nullptr, protocolInfo.get(), &bufferLength, &error);

	for (int i = 0; i < count; ++i)
	{
		if (protocolInfo[i].ProtocolChain.ChainLen == 1)
			::OutputDebugString(L"Base Service Provider: ");
		else
			::OutputDebugString(L"Layered Chain Entry: ");

		::OutputDebugString(protocolInfo[i].szProtocol);

		switch (protocolInfo[i].iSocketType)
		{
		case SOCK_STREAM:
			::OutputDebugString(L" SOCK_STREAM");
			break;
		case SOCK_DGRAM:
			::OutputDebugString(L" SOCK_DGRAM");
			break;
		case SOCK_RAW:
			::OutputDebugString(L" SOCK_RAW");
			break;
		case SOCK_RDM:
			::OutputDebugString(L" SOCK_RDM");
			break;
		case SOCK_SEQPACKET:
			::OutputDebugString(L" SOCK_SEQPACKET");
			break;
		}

		::OutputDebugString(L"\r\n");
	}

	return false;
}

WinSock::WinSock()
{
	WSADATA wsaData{};

	if (int error = ::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		FAIL(what(error));
	}
	else
	{
		INFO(HIBYTE(wsaData.wHighVersion), LOBYTE(wsaData.wHighVersion));
	}
}

WinSock::~WinSock()
{
	::WSACleanup();
}
