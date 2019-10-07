#pragma once

#include "stdafx.h"

struct WinSock
{
	WinSock();
	~WinSock();

	bool CheckRawSocketSupport();
};

class RawSocketsMainWindow
{
	WinSock winsock;

public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
