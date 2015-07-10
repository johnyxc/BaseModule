#ifndef __THREAD_POOL_HPP_2015_06_08__
#define __THREAD_POOL_HPP_2015_06_08__
#include <event2/event.h>
#include <bio.hpp>
#include <thread.hpp>
#include <function.hpp>
#include <bind.hpp>
#include <auto_ptr.hpp>
#include <strand.hpp>
#include <map>
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
				pt_param() : pbase_(), pthread_(), alive_(false) {}
				auto_ptr<thread_t> pthread_;
				event_base* pbase_;
				bool alive_;
			};

		public :
			thread_pool_t() : count_(2), cur_idx_() {}
			~thread_pool_t() {}

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
					pthread_[i].pthread_ = bas::make_auto_ptr<thread_t>(bind(&thread_pool_t::i_on_thread, this, &pthread_[i]));
					pthread_[i].pthread_->run();
				}
			}

			void stop()
			{
				std::map<event*, STANDARD_FUN*>::iterator iter;
				for(iter = io_event_list_.begin(); iter != io_event_list_.end(); ++iter)
				{
					event_del(iter->first);
					delete iter->second;
				}

				for(int i = 0; i < count_; i++)
				{
					pthread_[i].alive_ = false;
					pthread_[i].pthread_->join();
				}
			}

			void post(strand_t* strand)
			{
				event_base* pbase = pthread_[(cur_idx_++) % count_].pbase_;
				if(!pbase) return;
				event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)strand, 0);
			}

			void post(const function<void ()>& fo)
			{
				function<void ()>* pfo = new function<void ()>;
				*pfo = fo;

				event_base* pbase = pthread_[(cur_idx_++) % count_].pbase_;
				if(!pbase) return;
				event_base_once(pbase, 0, EV_TIMEOUT, i_on_event, (void*)pfo, 0);
			}

			event* get_event(evutil_socket_t sock, short type, const STANDARD_FUN& fo)
			{
				STANDARD_FUN* pfo = new STANDARD_FUN;
				*pfo = fo;

				event_base* pbase = pthread_[(cur_idx_++) % count_].pbase_;
				event* evt = event_new(pbase, sock, type, i_on_io_event, pfo);
				io_event_list_.insert(std::pair<event*, STANDARD_FUN*>(evt, pfo));

				return evt;
			}

			void post(event* evt)
			{
				event_add(evt, 0);
			}

			void remove(event* evt)
			{
				std::map<event*, STANDARD_FUN*>::iterator iter;
				iter = io_event_list_.find(evt);
				if(iter == io_event_list_.end()) return;
				event_del(iter->first);
				delete iter->second;
				io_event_list_.erase(iter);
			}

		private :
			void i_on_thread(pt_param* pt)
			{
				if(!pt) return;
				printf("Thread Start Up\n");
				while(pt->alive_)
				{
					event_base_dispatch(pt->pbase_);
					Sleep(1);
				}
				printf("Thread Exit\n");
			}

			static void i_on_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				function<void ()>* pfo = (function<void ()>*)arg;
				(*pfo)();
				delete pfo;
			}

			static void i_on_io_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				STANDARD_FUN* pfo = (STANDARD_FUN*)arg;
				(*pfo)(sock, type);
			}

		private :
			pt_param pthread_[MAX_THREAD_COUNT];
			std::map<event*, STANDARD_FUN*> io_event_list_;
			int count_;
			unsigned int cur_idx_;
		};
	}

	detail::auto_ptr<detail::thread_pool_t> tp;
	detail::thread_pool_t* default_thread_pool() { return tp.raw_ptr(); }
}

#endif
