#ifndef __OSFUNC_HPP_2015_06_23__
#define __OSFUNC_HPP_2015_06_23__

//////////////////////////////////////////////////////////////////////////
//	一些句柄定义
typedef void* HMUTEX;

//////////////////////////////////////////////////////////////////////////
//	实现
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
	CRITICAL_SECTION* cs = new CRITICAL_SECTION;
	::InitializeCriticalSection(cs);
	return (HMUTEX)cs;
}

void release_mutex(HMUTEX mutex)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mutex;
	::DeleteCriticalSection(cs);
	delete cs;
}

void lock(HMUTEX mutex)
{
	::EnterCriticalSection((CRITICAL_SECTION*)mutex);
}

bool try_lock(HMUTEX mutex)
{
	return ::TryEnterCriticalSection((CRITICAL_SECTION*)mutex);
}

void unlock(HMUTEX mutex)
{
	::LeaveCriticalSection((CRITICAL_SECTION*)mutex);
}

void bas_sleep(unsigned int ms)
{
    Sleep(ms);
}

#else	//	Linux

#include <pthread.h>
#include <unistd.h>

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
