#include "stdafx.h"
#include <init.h>
#include <set>
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
//////////////////////////////////////////////////////////////////////////

struct test_tag;
bas::detail::thread_pool_t* bas::detail::thread_pool_t::self_ = 0;
//bas::detail::mem_pool_manager_t* bas::detail::mem_pool_manager_t::self_ = 0;
std::vector<test_tag*>* g_pointer_map;

void init(int count)
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);
#else
#endif

	g_pointer_map = new std::vector<test_tag*>;
	bas::detail::thread_pool_t* tp = bas::default_thread_pool();
	tp->set_thread_count(count);
	tp->run();

	//bas::detail::mem_pool_manager_t* mp = bas::detail::mem_pool_manager_t::instance();
	//mp->init();
}

void uninit()
{
#ifdef _WIN32
	WSACleanup();
#else
#endif

	delete g_pointer_map;
	bas::detail::thread_pool_t* tp = bas::default_thread_pool();
	tp->stop();
}

win32_auto_init::win32_auto_init(int count) { init(count); }
win32_auto_init::~win32_auto_init() { uninit(); }
