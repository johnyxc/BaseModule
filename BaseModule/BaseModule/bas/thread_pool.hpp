#ifndef __THREAD_POOL_HPP_2015_06_08__
#define __THREAD_POOL_HPP_2015_06_08__
#include <event2/event.h>
#include <bio.hpp>
#include <thread.hpp>
#include <function.hpp>
#include <bind.hpp>
#include <auto_ptr.hpp>
#include <osfunc.hpp>
#include <memory.hpp>
#include <strand.hpp>
#include <vector>
#include <list>
#include <algorithm>

//	线程池基于 libevent 实现
//	这样一方面可以给 io 提供异步服务
//	另一方面可以给内部逻辑提供异步服务
namespace bas
{
	namespace detail
	{
		struct thread_pool_t : bio_bas_t<thread_pool_t>
		{
			enum { MAX_THREAD_COUNT = 10 };
			typedef function<void (evutil_socket_t, short)> STANDARD_FUN;

			//	线程核心结构
			struct pt_param
			{
				pt_param() : pthread_(), pbase_(), alive_(false) {}
				thread_t* pthread_;
				event_base* pbase_;
				bool alive_;
			};

			//	一次性事件结构
			struct exe_param
			{
				function<void ()>* pfo;
				int ms;
			};

		public :
			thread_pool_t() : count_(2), cur_idx_(), mutex_()
			{
				mutex_ = get_mutex();
				mem_zero((void*)&cur_ep_, sizeof(exe_param));
			}
			~thread_pool_t() { if(mutex_) release_mutex(mutex_); }

		public :
			void set_thread_count(int count = 2)
			{
				count_ = (count > MAX_THREAD_COUNT) ? MAX_THREAD_COUNT : count;
			}

			void run()
			{
				for(int i = 0; i < count_; i++)
				{
					pthread_[i].alive_ = true;
					pthread_[i].pbase_ = event_base_new();
					pthread_[i].pthread_ = mem_create_object<thread_t>(bind(&thread_pool_t::i_on_thread, bas::retain(this), &pthread_[i]));
					pthread_[i].pthread_->run();
				}
			}

			void stop()
			{
				for(int i = 0; i < count_; i++)
				{
					pthread_[i].alive_ = false;
					//pthread_[i].pthread_->join();
					mem_delete_object(pthread_[i].pthread_);
				}
			}

			//	暂不支持
			void post(strand_t* strand)
			{
				event_base* pbase = pthread_[(atom_inc(&cur_idx_)) % count_].pbase_;
				if(!pbase) return;
				lock(mutex_);
				event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)strand, 0);
				unlock(mutex_);
			}

			void post(const function<void ()> fo, int ms)
			{
				function<void ()>* pfo = mem_create_object<function<void ()> >();
				*pfo = fo;

				event_base* pbase = pthread_[(atom_inc(&cur_idx_)) % count_].pbase_;
				if(!pbase) return;

				if(ms == 0) {
					lock(mutex_);
					if(!cur_ep_.pfo) {
						cur_ep_.pfo = pfo;
						cur_ep_.ms = 0;
						event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)pfo, 0);
					} else {
						exe_param ep = {};
						ep.pfo = pfo;
						ep.ms = 0;
						once_event_list_.push_back(ep);
					}
					unlock(mutex_);
				} else {
					long sec = ms / 1000;
					long ms_remain = ms - sec * 1000;
					struct timeval tv;
					tv.tv_sec = sec;
					tv.tv_usec = ms_remain * 1000;
					lock(mutex_);
					event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)pfo, &tv);
					unlock(mutex_);
				}
			}

			event* get_event(evutil_socket_t sock, short type, const STANDARD_FUN& fo)
			{
				STANDARD_FUN* pfo = mem_create_object<STANDARD_FUN>();
				*pfo = fo;

				event_base* pbase = pthread_[(atom_inc(&cur_idx_)) % count_].pbase_;
				lock(mutex_);
				event* evt = event_new(pbase, sock, type, i_on_io_event, pfo);
				unlock(mutex_);

				return evt;
			}

			void post(event* evt)
			{
				lock(mutex_);
				event_add(evt, 0);
				unlock(mutex_);
			}

			void post(event* evt, struct timeval* tv)
			{
				lock(mutex_);
				event_add(evt, tv);
				unlock(mutex_);
			}

			void remove(event* evt)
			{
				lock(mutex_);
				STANDARD_FUN* pfo = (STANDARD_FUN*)event_get_callback_arg(evt);
				if(pfo) mem_delete_object(pfo);
				event_del(evt);
				event_free(evt);
				unlock(mutex_);
			}

		private :
			void i_on_thread(pt_param* pt)
			{
				if(!pt) return;
				printf("Thread Start Up\n");
				{	//	libevent有个非常奇怪的问题, 有未响应事件时,
					//	一次性事件投递无效, 这里强制让一次性事件循环,
					//	以驱动其他逻辑事件, 将来有时间继续排查此Bug.
					struct timeval tv;
					tv.tv_sec = 0;
					tv.tv_usec = 100 * 1000;
					event_base_once(pt->pbase_, 0, EV_TIMEOUT, i_on_helper, 0, &tv);
				}
				while(pt->alive_)
				{
					event_base_dispatch(pt->pbase_);
					bas_sleep(1);
				}
				printf("Thread Exit\n");
			}

			static void i_on_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				function<void ()>* pfo = (function<void ()>*)arg;
				(*pfo)();
				mem_delete_object(pfo);

				exe_param ep = {};

				lock(thread_pool_t::instance()->mutex_);
				thread_pool_t::instance()->cur_ep_ = ep;
				if(thread_pool_t::instance()->once_event_list_.size())
				{
					ep = thread_pool_t::instance()->once_event_list_.front();
					thread_pool_t::instance()->once_event_list_.pop_front();
					if(ep.pfo) thread_pool_t::instance()->cur_ep_ = ep;
				}
				unlock(thread_pool_t::instance()->mutex_);

				if(ep.pfo)
				{
					event_base* pbase = thread_pool_t::instance()->pthread_[(atom_inc(&thread_pool_t::instance()->cur_idx_)) % thread_pool_t::instance()->count_].pbase_;
					if(!pbase) return;
					event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)ep.pfo, 0);
				}
			}

			static void i_on_io_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				STANDARD_FUN* pfo = (STANDARD_FUN*)arg;
				(*pfo)(sock, type);
			}

			static void i_on_helper(evutil_socket_t sock, short type, void* arg)
			{
				event_base* pbase = thread_pool_t::instance()->pthread_[(atom_inc(&thread_pool_t::instance()->cur_idx_)) % thread_pool_t::instance()->count_].pbase_;

				if(!(thread_pool_t::instance()->pthread_[(atom_inc(&thread_pool_t::instance()->cur_idx_)) % thread_pool_t::instance()->count_].alive_))
				{
					event_base_loopbreak(pbase);
					return;
				}

				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 10 * 1000;
				event_base_once(pbase, 0, EV_TIMEOUT, i_on_helper, 0, &tv);
			}

		public :
			pt_param	pthread_[MAX_THREAD_COUNT];
			int			count_;
			long		cur_idx_;
			HMUTEX		mutex_;
			exe_param	cur_ep_;

		public :
			static thread_pool_t* instance()
			{
				if(!self_) self_ = mem_create_object<thread_pool_t>();
				return self_;
			}

		private :
			static thread_pool_t* self_;
			std::list<exe_param> once_event_list_;
		};
	}

	static detail::thread_pool_t* default_thread_pool() { return detail::thread_pool_t::instance(); }
}

#endif
