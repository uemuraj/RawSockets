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
		wc.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return p->WindowProc(hwnd, uMsg, wParam, lParam); };
		wc.lpszClassName = MainWindowClassName;

		wc.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
		wc.hIconSm = ::LoadIconW(nullptr, IDI_APPLICATION);
		wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);

		return mainWindow.Create(hInstance, LPCWSTR(::RegisterClassEx(&wc)), MainWindowTitleText);
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int nCmdShow)
{
	MainWindowClass mainWindow;

	if (HWND hwnd = CreateMainWindow(hInstance, mainWindow))
	{
		::ShowWindow(hwnd, nCmdShow);
		::UpdateWindow(hwnd);

		MSG msg{};

		while (::GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}
