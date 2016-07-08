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

//	�̳߳ػ��� libevent ʵ��
//	����һ������Ը� io �ṩ�첽����
//	��һ������Ը��ڲ��߼��ṩ�첽����
namespace bas
{
	namespace detail
	{
		struct thread_pool_t : bio_bas_t<thread_pool_t>
		{
			enum { MAX_THREAD_COUNT = 10 };
			typedef function<void (evutil_socket_t, short)> STANDARD_FUN;
			typedef function<void ()>						ONCE_FUN;

			//	�̺߳��Ľṹ
			struct pt_param
			{
				pt_param() : pthread_get(), alive(false) {}
				thread_t* pthread_get;
				HEVENT evt;
				bool alive;
			};

			//	һ�����¼��ṹ
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

			//	ͨ���˺���Ͷ�ݵĺ���������Ҫ�ṩ strand ����
			//	ͬһ�� strand �µĺ�������֤����ִ�У���ʹ�ڶ��̻߳�����Ҳ�����
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

			//	ͨ���˺���Ͷ�ݵĺ�������ִ��˳�򲻶�
			//	���ڶ��̻߳����´��ڲ���ִ�еĿ���
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

			//	����һ�� io �¼�
			event* get_event(evutil_socket_t sock, short type, const STANDARD_FUN& fo)
			{
				STANDARD_FUN* pfo = mem_create_object<STANDARD_FUN>();
				*pfo = fo;
				event* evt = event_new(pbase_, sock, type, i_on_io_event, pfo);
				return evt;
			}

			//	һ�� io �¼�ͨ���˺���Ͷ��ִ��
			void post(event* evt)
			{
				event_add(evt, 0);
			}

			void post(event* evt, struct timeval* tv)
			{
				event_add(evt, tv);
			}

			//	�ͷ��� get_event ������ io �¼�
			void remove(event* evt)
			{
				STANDARD_FUN* pfo = (STANDARD_FUN*)event_get_callback_arg(evt);
				if(pfo) mem_delete_object(pfo);
				event_del(evt);
				event_free(evt);
			}

		private :
			//	�߳�ִ����
			//	���� libevent ʹ�õ��̣߳��Ǵ˶���ĺ���
			//	���е� io ���ڲ��� strand һ�����¼����ɴ��߳�����
			void i_on_thread()
			{
				printf("Thread Start Up\n");
				{	//	libevent �и��ǳ���ֵ�����, ��δ��Ӧ�¼�ʱ,
					//	һ�����¼�Ͷ����Ч, ����ǿ����һ�����¼�ѭ��,
					//	�����������߼��¼�, ������ʱ������Ų��Bug.
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

			//	��������ִ���߳�
			//	���ܻ��ж���̣߳���ͬһ�����ȶ���
			//	��ȡ��һ������ִ�У��Զ��󱻴���
			//	��ʱ��������ȼ�������ʱ����Ķ���
			//	ӵ�нϸ����ȼ��������ȱ�ѡ��ִ��
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

			//	����һ���߳����ڽ������ȶ���
			//	����ͬһ�� strand �ĺ�����������
			//	����ģ�����ͬʱ���������ȶ�����
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

			//	һ�����¼���ʱ�¼���Ӧ����
			static void i_on_event(evutil_socket_t sock, short type, void* arg)
			{
				if(!arg) return;
				function<void ()>* pfo = (function<void ()>*)arg;
				(*pfo)();
				mem_delete_object(pfo);
			}

			//	io �¼���Ӧ����
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
