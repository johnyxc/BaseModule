#ifndef __OSFUNC_HPP_2015_06_23__
#define __OSFUNC_HPP_2015_06_23__

//////////////////////////////////////////////////////////////////////////
//	һЩ�������
typedef void* HMUTEX;

//////////////////////////////////////////////////////////////////////////
//	ʵ��
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
    pthread_mutex_t* mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex, 0);
	return (HMUTEX)mutex;
}

void release_mutex(HMUTEX mutex)
{
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
}

void lock(HMUTEX mutex)
{
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

bool try_lock(HMUTEX mutex)
{
	return (pthread_mutex_trylock((pthread_mutex_t*)mutex) == 0);
}

void unlock(HMUTEX mutex)
{
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

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
