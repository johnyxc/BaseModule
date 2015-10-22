#ifndef __ACTIVEOBJECT_HPP_2015_06_03__
#define __ACTIVEOBJECT_HPP_2015_06_03__
#include <bio.hpp>
#include <thread_pool.hpp>
#include <strand.hpp>

namespace bas
{
	namespace detail
	{
		template <typename T>
		struct active_object_t : bio_bas_t<T>
		{
		public :
			active_object_t() : strand_() {}
			~active_object_t() { if(strand_) strand_->release(); }

		public :
			strand_t* get_strand()
			{
				if(!strand_) strand_ = mem_create_object<strand_t>();
				return strand_;
			}

			void set_strand(strand_t* strand)
			{
				if(strand_) strand_->release();
				strand_ = strand;
				if(strand_) strand_->retain();
			}

			void post_call(const function<void ()>& fo)
			{
				default_thread_pool()->post_call(fo, get_strand());
			}

		private :
			strand_t* strand_;
		};
	}
}

#endif
