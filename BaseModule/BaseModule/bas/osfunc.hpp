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

static int atom_inc(long* v)
{
	return InterlockedIncrement(v);
}

static int atom_sub(long* v)
{
	return InterlockedDecrement(v);
}

static int atom_exchage(long* t, long v)
{
	return InterlockedExchange(t, v);
}

static HMUTEX get_mutex()
{
	CRITICAL_SECTION* cs = new CRITICAL_SECTION;
	::InitializeCriticalSection(cs);
	return (HMUTEX)cs;
}

static void release_mutex(HMUTEX mutex)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mutex;
	::DeleteCriticalSection(cs);
	delete cs;
}

static void lock(HMUTEX mutex)
{
	::EnterCriticalSection((CRITICAL_SECTION*)mutex);
}

static bool try_lock(HMUTEX mutex)
{
	return ::TryEnterCriticalSection((CRITICAL_SECTION*)mutex);
}

static int get_own_count(HMUTEX mutex)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mutex;
	return cs->RecursionCount;
}

static void unlock(HMUTEX mutex)
{
	::LeaveCriticalSection((CRITICAL_SECTION*)mutex);
}

static void bas_sleep(unsigned int ms)
{
    Sleep(ms);
}

struct auto_lock_t
{
	auto_lock_t(HMUTEX mtx) : mutex_(mtx) { lock(mutex_); }
	~auto_lock_t() { unlock(mutex_); }

private :
	HMUTEX mutex_;
};

#define AUTOLOCK(mtx) \
	auto_lock_t al(mtx)

#else	//	Linux

#include <pthread.h>
#include <unistd.h>

static HMUTEX get_mutex()
{
    pthread_mutex_t* mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex, 0);
	return (HMUTEX)mutex;
}

static void release_mutex(HMUTEX mutex)
{
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
}

static void lock(HMUTEX mutex)
{
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

static bool try_lock(HMUTEX mutex)
{
	return (pthread_mutex_trylock((pthread_mutex_t*)mutex) == 0);
}

static void unlock(HMUTEX mutex)
{
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

static int atom_inc(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

static int atom_sub(long* v)
{
	return __sync_fetch_and_add(v, 1);
}

static int atom_exchage(long* t, long v)
{
	return 0;
}

static void bas_sleep(unsigned int ms)
{
    usleep(ms * 1000);
}

#endif	//	_WIN32
#endif	//	__OSFUNC_HPP_2015_06_23__
