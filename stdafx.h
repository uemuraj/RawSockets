#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#include <ws2spi.h>
//#include <ws2tcpip.h>

#if !defined(UNICODE)
#error
#endif

#include <string>
