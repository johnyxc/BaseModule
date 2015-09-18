#ifndef __TIMER_HPP_2015_08_27__
#define __TIMER_HPP_2015_08_27__
#include <bio.hpp>
#include <thread_pool.hpp>
#include <function.hpp>
#include <auto_ptr.hpp>
#include <event2/event.h>

namespace bas
{
	namespace detail
	{
		struct set_timer_info : bio_bas_t<set_timer_info>
		{
			function<void ()> timer_cb_;
		};

		struct timer_t : bas::detail::bio_bas_t<timer_t>
		{
		public :
			timer_t() : ev_timer_() {}
			~timer_t() { if(ev_timer_) default_thread_pool()->remove(ev_timer_); }

		public :
			void wait(function<void ()> cb, int ms)
			{
				auto_ptr<set_timer_info> sti = mem_create_object<set_timer_info>();
				sti->timer_cb_ = cb;

				long sec = ms / 1000;
				long ms_remain = ms - sec * 1000;
				struct timeval tv;
				tv.tv_sec  = sec;
				tv.tv_usec = ms_remain * 1000;

				if(ev_timer_) if(ev_timer_) default_thread_pool()->remove(ev_timer_);
				ev_timer_ = default_thread_pool()->get_event(-1, 0, bind(&timer_t::time_cb, bas::retain(this), _1, _2, sti));
				if(ev_timer_) default_thread_pool()->post(ev_timer_, &tv);
			}

			void cancel()
			{
				if(ev_timer_) default_thread_pool()->remove(ev_timer_);
				ev_timer_ = 0;
			}

			void time_cb(evutil_socket_t, short, auto_ptr<set_timer_info> sti)
			{
				if(sti.valid())
				{
					sti->timer_cb_();
				}
			}

		private :
			struct event* ev_timer_;
		};
	}
}

#endif
