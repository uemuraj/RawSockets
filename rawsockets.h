#pragma once

#include "stdafx.h"

class RawSocketsMainWindow
{
public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool CheckRawSocketSupport();
};

class WinSock
{
public:
	WinSock();
	~WinSock();
};