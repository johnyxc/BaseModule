#ifndef __INIT_HPP_2015_07_20__
#define __INIT_HPP_2015_07_20__

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#endif

#ifdef _DEBUG
#pragma comment(lib, ".\\libevent\\lib\\Debug\\libevent.lib")
#else
#pragma comment(lib, ".\\libevent\\lib\\Release\\libevent.lib")
#endif

#include <thread_pool.hpp>
#include <mem_pool.hpp>
extern bas::detail::thread_pool_t* bas::tp;
extern bas::detail::mem_pool_manager_t* bas::mem_pool;
//////////////////////////////////////////////////////////////////////////

void init(int count)
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);
#else
#endif

	bas::tp = mem_create_object<bas::detail::thread_pool_t>();
	bas::tp->set_thread_count(count);
	bas::tp->run();

	bas::mem_pool->init();
}

void uninit()
{
#ifdef _WIN32
	WSACleanup();
#else
#endif
}

struct win32_auto_init
{
	win32_auto_init(int count) { init(count); }
	~win32_auto_init() { uninit(); }
};

#define bas_init(c) \
	win32_auto_init wai(c); \

#endif	//	__INIT_HPP_2015_07_20__
