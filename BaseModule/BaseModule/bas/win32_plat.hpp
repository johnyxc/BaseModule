#ifndef __WIN32_PLAT_HPP_2015_06_10__
#define __WIN32_PLAT_HPP_2015_06_10__

#ifdef _WIN32

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, ".\\libevent\\lib\\Debug\\libevent.lib")
#else
#pragma comment(lib, ".\\libevent\\lib\\Release\\libevent.lib")
#endif

void win32_init()
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);
}

void win32_uninit()
{
	WSACleanup();
}

struct win32_auto_init
{
	win32_auto_init() { win32_init(); }
	~win32_auto_init() { win32_uninit(); }
} wai;

#endif
#endif
