#ifndef __STRAND_HPP_2015_06_08__
#define __STRAND_HPP_2015_06_08__
#include <bio.hpp>
#include <function.hpp>
#include <osfunc.hpp>

namespace bas
{
	namespace detail
	{
		struct strand_t : bas::detail::bio_bas_t<strand_t>
		{
		public :
			strand_t() : mutex_() { mutex_ = get_mutex(); }
			~strand_t() { release_mutex(mutex_); }

		public :
			void enter_section()
			{
				lock(mutex_);
			}

			bool try_enter_section()
			{
				return try_lock(mutex_);
			}

			int get_section_own_count()
			{
				return get_own_count(mutex_);
			}

			void leave_section()
			{
				unlock(mutex_);
			}

		private :
			HMUTEX mutex_;
		};
	}
}

#endif
