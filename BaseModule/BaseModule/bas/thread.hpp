#ifndef __THREAD_HPP_2015_06_03__
#define __THREAD_HPP_2015_06_03__
#include <auto_ptr.hpp>
#include <function.hpp>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#endif
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		struct thread_t : bio_bas_t<thread_t>
		{
		public :
			explicit thread_t(function<void()> fo) : fo_(fo), handle_(), tid_() {}
			~thread_t() { if(handle_) join(); }

		public :
			bool run()
			{
#ifdef _WIN32
				handle_ = ::CreateThread(0, 0, thread_wrap, this, 0, (LPDWORD)&tid_);
				return handle_ != 0;
#endif
			}

			void join()
			{
#ifdef _WIN32
				::WaitForSingleObject(handle_, INFINITE);
#endif
			}

			void kill()
			{
#ifdef _WIN32
				::TerminateThread(handle_, 0);
#endif
			}

		private :
			static DWORD WINAPI _stdcall thread_wrap(LPVOID param)
			{
				thread_t* pThis = (thread_t*)param;
				pThis->fo_();
				return 0;
			}

		private :
			function<void()> fo_;
			int tid_;
#ifdef _WIN32
			HANDLE handle_;
#endif
		};
	}
}

#endif
