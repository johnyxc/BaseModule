#ifndef __WIN32_PLAT_HPP_2015_06_10__
#define __WIN32_PLAT_HPP_2015_06_10__

#ifdef _WIN32

#include <WinSock2.h>
#include <thread_pool.hpp>
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, ".\\libevent\\lib\\Debug\\libevent.lib")
#else
#pragma comment(lib, ".\\libevent\\lib\\Release\\libevent.lib")
#endif

void init()
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);

	bas::tp = bas::make_auto_ptr<bas::detail::thread_pool_t>();
}

void uninit()
{
	WSACleanup();
}

struct win32_auto_init
{
	win32_auto_init() { init(); }
	~win32_auto_init() { uninit(); }
};

#define bas_init() \
	static win32_auto_init wai

#endif	//	_WIN32
#endif	//	__WIN32_PLAT_HPP_2015_06_10__
