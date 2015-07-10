#ifndef __OSFUNC_HPP_2015_06_23__
#define __OSFUNC_HPP_2015_06_23__

//////////////////////////////////////////////////////////////////////////
//	一些句柄定义
typedef void* HMUTEX;

//////////////////////////////////////////////////////////////////////////
//	对外接口
static void init(int count);
static void uninit();
static int atom_inc(long* v);
static int atom_sub(long* v);
static HMUTEX get_mutex();
static void release_mutex(HMUTEX);
static void lock(HMUTEX);
static void unlock(HMUTEX);
static void bas_sleep(unsigned int ms);

//////////////////////////////////////////////////////////////////////////
//	实现
#ifdef _WIN32

#include <WinSock2.h>
#include <thread_pool.hpp>
#pragma comment(lib, "ws2_32.lib")
extern bas::detail::auto_ptr<bas::detail::thread_pool_t> bas::tp;

#ifdef _DEBUG
#pragma comment(lib, ".\\libevent\\lib\\Debug\\libevent.lib")
#else
#pragma comment(lib, ".\\libevent\\lib\\Release\\libevent.lib")
#endif

void init(int count)
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);

	bas::tp = bas::make_auto_ptr<bas::detail::thread_pool_t>();
	bas::tp->set_thread_count(count);
	bas::tp->run();
}

void uninit()
{
	WSACleanup();
}

struct win32_auto_init
{
	win32_auto_init(int count) { init(count); }
	~win32_auto_init() { uninit(); }
};

#define bas_init(c) \
	win32_auto_init wai(c); \

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

void bas_sleep(unsigned int ms)
{
    Sleep(ms);
}

#else	//	Linux

#include <pthread.h>
#include <unistd.h>

void init(int count)
{}

void uninit()
{}

HMUTEX get_mutex()
{
    return 0;
}

void release_mutex(HMUTEX mutex)
{}

void lock(HMUTEX mutex)
{}

void unlock(HMUTEX mutex)
{}

int atom_inc(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

int atom_sub(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

void bas_sleep(unsigned int ms)
{
    usleep(ms * 1000);
}

#endif	//	_WIN32
#endif	//	__OSFUNC_HPP_2015_06_23__
