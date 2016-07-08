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
#include <list>
#include <map>
#include <queue>

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
			typedef function<void ()>						ONCE_FUN;

			//	线程核心结构
			struct pt_param
			{
				pt_param() : pthread_get(), alive(false) {}
				thread_t* pthread_get;
				HEVENT evt;
				bool alive;
			};

			//	一次性事件结构
			struct exe_param
			{
				ONCE_FUN pfo;
				strand_t* strand;
				long long time;
				friend bool operator < (const exe_param& a, const exe_param& b) { return a.time < b.time; }
			};

		public :
			thread_pool_t() : pbase_(),
				base_thread_(),
				base_thread_alive_(false),
				count_(2),
				evt_(),
				list_mutex_(),
				own_list_mutex_(),
				build_thread_(),
				build_thread_alive_(false)
			{
				list_mutex_		= get_mutex();
				own_list_mutex_ = get_mutex();
				evt_ = get_event_hdl(false, false);
			}
			~thread_pool_t()
			{
				if(list_mutex_)		release_mutex(list_mutex_);
				if(own_list_mutex_) release_mutex(own_list_mutex_);
				if(evt_)			release_event_hdl(evt_);
			}

		public :
			void set_thread_count(int count = 2)
			{
				count_ = (count > MAX_THREAD_COUNT) ? MAX_THREAD_COUNT : count;
			}

			void run()
			{
				pbase_ = event_base_new();
				base_thread_alive_ = true;
				base_thread_ = mem_create_object<thread_t>(bind(&thread_pool_t::i_on_thread, bas::retain(this)));
				build_thread_alive_ = true;
				build_thread_ = mem_create_object<thread_t>(bind(&thread_pool_t::i_on_thread_build, bas::retain(this)));

				for(int i = 0; i < count_; i++)
				{
					pthread_[i].alive = true;
					pthread_[i].evt = get_event_hdl(false, false);
					pthread_[i].pthread_get = mem_create_object<thread_t>(bind(&thread_pool_t::i_on_thread_get, bas::retain(this), &pthread_[i]));
					pthread_[i].pthread_get->run();
				}

				base_thread_->run();
				build_thread_->run();
			}

			void stop()
			{
				for(int i = 0; i < count_; i++)
				{
					pthread_[i].alive = false;
					set_event(pthread_[i].evt);
					pthread_[i].pthread_get->join();
					pthread_[i].pthread_get->release();
				}

				base_thread_alive_ = false;
				build_thread_alive_ = false;
				event_base_loopexit(pbase_, 0);
				base_thread_->join();
				build_thread_->join();
				base_thread_->release();
				build_thread_->release();
			}

			//	通过此函数投递的函数对象需要提供 strand 参数
			//	同一个 strand 下的函数对象保证串行执行，即使在多线程环境下也是如此
			void post_call(const function<void ()> fo, strand_t* strand)
			{
				if(!strand) return;

				exe_param ep = {};
				ep.pfo = fo;
				ep.strand = strand;
				ep.strand->retain();
				ep.time = GetTickCount();

				std::map<strand_t*, std::list<exe_param> >::iterator iter;
				lock(own_list_mutex_);
				iter = own_event_list_.find(strand);
				if(iter == own_event_list_.end())
				{
					std::list<exe_param> exe_list;
					exe_list.push_back(ep);
					own_event_list_.insert(std::pair<strand_t*, std::list<exe_param> >(strand, exe_list));
				}
				else
				{
					iter->second.push_back(ep);
				}
				unlock(own_list_mutex_);

				set_event(evt_);
			}

			//	通过此函数投递的函数对象执行顺序不定
			//	且在多线程环境下存在并发执行的可能
			void post(const function<void ()> fo, int ms)
			{
				function<void ()>* pfo = mem_create_object<function<void ()> >();
				*pfo = fo;

				long sec = ms / 1000;
				long ms_remain = ms - sec * 1000;
				struct timeval tv;
				tv.tv_sec = sec;
				tv.tv_usec = ms_remain * 1000;
				event_base_once(pbase_, 0, EV_TIMEOUT, i_on_event, (void*)pfo, &tv);
			}

			//	创建一个 io 事件
			event* get_event(evutil_socket_t sock, short type, const STANDARD_FUN& fo)
			{
				STANDARD_FUN* pfo = mem_create_object<STANDARD_FUN>();
				*pfo = fo;
				event* evt = event_new(pbase_, sock, type, i_on_io_event, pfo);
				return evt;
			}

			//	一般 io 事件通过此函数投递执行
			void post(event* evt)
			{
				event_add(evt, 0);
			}

			void post(event* evt, struct timeval* tv)
			{
				event_add(evt, tv);
			}

			//	释放由 get_event 创建的 io 事件
			void remove(event* evt)
			{
				STANDARD_FUN* pfo = (STANDARD_FUN*)event_get_callback_arg(evt);
				if(pfo) mem_delete_object(pfo);
				event_del(evt);
				event_free(evt);
			}

		private :
			//	线程执行体
			//	这是 libevent 使用的线程，是此对象的核心
			//	所有的 io 或内部非 strand 一次性事件都由此线程驱动
			void i_on_thread()
			{
				printf("Thread Start Up\n");
				{	//	libevent 有个非常奇怪的问题, 有未响应事件时,
					//	一次性事件投递无效, 这里强制让一次性事件循环,
					//	以驱动其他逻辑事件, 将来有时间继续排查此Bug.
					struct timeval tv;
					tv.tv_sec = 0;
					tv.tv_usec = 10;
					event_base_once(pbase_, 0, EV_TIMEOUT, i_on_helper, 0, &tv);
				}
				while(base_thread_alive_)
				{
					event_base_dispatch(pbase_);
					bas_sleep(1);
				}
				printf("Thread Exit\n");
			}

			//	函数对象执行线程
			//	可能会有多个线程，从同一个优先队列
			//	中取出一个对象执行，以对象被创建
			//	的时间计算优先级，创建时间早的对象
			//	拥有较高优先级，会优先被选出执行
			void i_on_thread_get(pt_param* pt)
			{
				if(!pt) return;
				printf("Thread Get Start Up\n");
				while(pt->alive)
				{
					exe_param ep = {};

					lock(list_mutex_);
					if(event_list_.size())
					{
						ep = event_list_.top();
						event_list_.pop();
						unlock(list_mutex_);
					}
					else
					{
						unlock(list_mutex_);
						event_wait(pt->evt);
						continue;
					}

					ep.pfo();
					ep.strand->leave_section();
					ep.strand->release();
				}
				printf("Thread Get Exit\n");
			}

			//	单独一个线程用于建立优先队列
			//	属于同一个 strand 的函数对象总是
			//	互斥的，不会同时出现在优先队列中
			void i_on_thread_build()
			{
				printf("Thread Build Start Up\n");
				while(build_thread_alive_)
				{
					lock(own_list_mutex_);
					std::map<strand_t*, std::list<exe_param> >::iterator iter;
					for(iter = own_event_list_.begin(); iter != own_event_list_.end(); )
					{
						if(iter->first->get_section_own_count() == 0)
						{
							exe_param ep = {};
							iter->first->enter_section();
							if(iter->second.size())
							{
								ep = iter->second.front();
								iter->second.pop_front();
								if(iter->second.empty()) {
									own_event_list_.erase(iter++);
								} else {
									++iter;
								}
							}
							else
							{
								iter->first->leave_section();
								own_event_list_.erase(iter++);
								unlock(own_list_mutex_);
								continue;
							}

							lock(list_mutex_);
							event_list_.push(ep);
							unlock(list_mutex_);
							for(int i = 0; i < count_; i++)
							{
								set_event(pthread_[i].evt);
							}
						}
						else
						{
							++iter;
						}
					}
					unlock(own_list_mutex_);
					if(own_event_list_.empty())
					{
						event_wait(evt_);
						continue;
					}
				}
				printf("Thread Build Exit\n");
			}

			//	一次性事件或定时事件响应函数
			static void i_on_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				function<void ()>* pfo = (function<void ()>*)arg;
				(*pfo)();
				mem_delete_object(pfo);
			}

			//	io 事件响应函数
			static void i_on_io_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				STANDARD_FUN* pfo = (STANDARD_FUN*)arg;
				(*pfo)(sock, type);
			}

			static void i_on_helper(evutil_socket_t sock, short type, void* arg)
			{
				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 10;
				event_base_once(thread_pool_t::instance()->pbase_, 0, EV_TIMEOUT, i_on_helper, 0, &tv);
			}

		public :
			event_base*	pbase_;
			thread_t*	base_thread_;
			bool		base_thread_alive_;
			pt_param	pthread_[MAX_THREAD_COUNT];
			int			count_;
			HMUTEX		evt_;
			HMUTEX		list_mutex_;
			HMUTEX		own_list_mutex_;
			thread_t*	build_thread_;
			bool		build_thread_alive_;
			std::priority_queue<exe_param> event_list_;
			std::map<strand_t*, std::list<exe_param> > own_event_list_;

		public :
			static thread_pool_t* instance()
			{
				if(!self_) self_ = mem_create_object<thread_pool_t>();
				return self_;
			}

		private :
			static thread_pool_t* self_;
		};
	}

	static detail::thread_pool_t* default_thread_pool() { return detail::thread_pool_t::instance(); }
}

#endif
