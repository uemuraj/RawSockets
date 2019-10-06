#include "stdafx.h"
#include "rawsockets.h"

namespace
{
	using MainWindowClass = RawSocketsMainWindow;
	wchar_t MainWindowClassName[] = L"RawSockets";
	wchar_t MainWindowTitleText[] = L"Learn to Program Windows";

	HWND CreateMainWindow(HINSTANCE hInstance, MainWindowClass & mainWindow)
	{
		static MainWindowClass * p = &mainWindow;

		WNDCLASSEX wc{ sizeof(wc) };

		wc.hInstance = hInstance;
		wc.lpfnWndProc = [] (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return p->WindowProc(hwnd, uMsg, wParam, lParam); };
		wc.lpszClassName = MainWindowClassName;

		// TODO: カーソル、背景色、アイコンの設定

		::RegisterClassEx(&wc);

		return ::CreateWindowEx(0, MainWindowClassName, MainWindowTitleText, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindowClass mainWindow;

	if (HWND hwnd = CreateMainWindow(hInstance, mainWindow))
	{
		::ShowWindow(hwnd, nCmdShow);

		MSG msg{};

		while (::GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}
