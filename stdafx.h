#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#include <commctrl.h>
#include <ws2spi.h>

#if !defined(UNICODE) || _MSVC_LANG < 201703L
#error
#endif

namespace
{
	class Registry
	{
		HKEY m_hKey{};

	public:
		Registry(const wchar_t * subkey)
		{
			::RegCreateKeyExW(HKEY_CURRENT_USER, subkey, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &m_hKey, nullptr);
		}

		~Registry()
		{
			::RegCloseKey(m_hKey);
		}

		template <typename V>
		void Get(const wchar_t * name, V & value)
		{
			DWORD size{ sizeof(value) };

			::RegQueryValueExW(m_hKey, name, nullptr, nullptr, (BYTE *) &value, &size);
		}

		template <typename V>
		void Set(const wchar_t * name, const V & value)
		{
			::RegSetValueExW(m_hKey, name, 0, REG_BINARY, (const BYTE *) &value, sizeof(value));
		}

		template <>
		void Set(const wchar_t * name, const long & value)
		{
			::RegSetValueExW(m_hKey, name, 0, REG_DWORD, (const BYTE *) &value, sizeof(value));
		}

		Registry(const Registry &) = delete;
		Registry & operator=(const Registry &) = delete;

		Registry(const Registry &&) = delete;
		Registry & operator=(const Registry &&) = delete;
	};

	inline RECT GetWindowRect(HWND hwnd)
	{
		RECT rect{};
		::GetWindowRect(hwnd, &rect);
		return rect;
	}

	inline SIZE GetClientRectSizeOffset(CREATESTRUCT * p)
	{
		RECT rect{ p->x, p->y, p->cx, p->cy };

		::AdjustWindowRectEx(&rect, p->style, p->hMenu != nullptr, p->dwExStyle);

		int offset_cx = (rect.left - p->x) - (rect.right - p->cx);
		int offset_cy = (rect.top - p->y) - (rect.bottom - p->cy);

		return { offset_cx, offset_cy };
	}

	template<typename T>
	SIZE GetClientRectSize(T size, SIZE offset)
	{
		return { size->cx + offset.cx, size->cy + offset.cy };
	}
}
