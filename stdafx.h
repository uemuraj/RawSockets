#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#include <ws2spi.h>

#if !defined(UNICODE) || _MSVC_LANG < 201703L
#error
#endif


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

	template <size_t SIZE>
	void Set(const wchar_t * name, const wchar_t(&value)[SIZE])
	{
		::RegSetValueExW(m_hKey, name, 0, REG_SZ, (const BYTE *) &value, sizeof(value));
	}
};
