#ifndef __OSFUNC_HPP_2015_06_23__
#define __OSFUNC_HPP_2015_06_23__

//////////////////////////////////////////////////////////////////////////
//	一些句柄定义
typedef void* HMUTEX;

//////////////////////////////////////////////////////////////////////////
//	对外接口
static void init();
static void uninit();
static int atom_inc(long* v);
static int atom_sub(long* v);
static HMUTEX get_mutex();
static void release_mutex(HMUTEX);
static void lock(HMUTEX);
static void unlock(HMUTEX);

//////////////////////////////////////////////////////////////////////////
//	实现
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

int atom_inc(long* v)
{
	return InterlockedIncrement(v);
}

int atom_sub(long* v)
{
	return InterlockedDecrement(v);
}

HMUTEX get_mutex()
{
	return (HMUTEX)::CreateMutex(0, false, 0);
}

void release_mutex(HMUTEX mutex)
{
	::CloseHandle(mutex);
}

void lock(HMUTEX mutex)
{
	::WaitForSingleObject((HANDLE)mutex, INFINITE);
}

void unlock(HMUTEX mutex)
{
	::ReleaseMutex((HANDLE)mutex);
}

#else	//	Linux

#include <pthread.h>

void init()
{}

void uninit()
{}

int atom_inc(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

int atom_sub(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

#endif	//	_WIN32
#endif	//	__OSFUNC_HPP_2015_06_23__
