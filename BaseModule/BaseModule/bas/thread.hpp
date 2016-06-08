#ifndef __THREAD_HPP_2015_06_03__
#define __THREAD_HPP_2015_06_03__
#include <auto_ptr.hpp>
#include <function.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		struct thread_t : bio_bas_t<thread_t>
		{
		public :
			explicit thread_t(function<void()> fo) : fo_(fo), tid_() {}
			~thread_t()
			{
#ifdef _WIN32
				if(handle_) CloseHandle(handle_);
#else
#endif
			}

		public :
			bool run()
			{
				retain();
#ifdef _WIN32
				handle_ = ::CreateThread(0, 0, thread_wrap, this, 0, (LPDWORD)&tid_);
				return handle_ != 0;
#else
				return (pthread_create(&th_, 0, thread_wrap, this) == 0);
#endif
			}

			void join()
			{
#ifdef _WIN32
				::WaitForSingleObject(handle_, INFINITE);
#else
				pthread_join(th_, 0);
#endif
			}

			void kill()
			{
#ifdef _WIN32
				::TerminateThread(handle_, 0);
#else
				pthread_cancel(th_);
#endif
			}

		private :
#ifdef _WIN32
			static DWORD WINAPI thread_wrap(LPVOID param)
			{
				thread_t* pThis = (thread_t*)param;
				pThis->fo_();
				pThis->release();
				return 0;
			}
#else
			static void* thread_wrap(void* arg)
			{
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
				thread_t* pThis = (thread_t*)arg;
				pThis->fo_();
				pthread_detach(pthread_self());
				pThis->release();
				return 0;
			}
#endif

		private :
			function<void()> fo_;
			int tid_;

#ifdef _WIN32
			HANDLE handle_;
#else
			pthread_t th_;
#endif
		};
	}
}

#endif
