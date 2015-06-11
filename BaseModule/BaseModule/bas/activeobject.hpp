#ifndef __ACTIVEOBJECT_HPP_2015_06_03__
#define __ACTIVEOBJECT_HPP_2015_06_03__
#include <thread_pool.hpp>
#include <bio.hpp>

namespace bas
{
	namespace detail
	{
		template <typename T>
		struct active_object_t : bio_bas_t<T>
		{
		public :
			active_object_t() {}
			~active_object_t() {}

		public :
			void post(const function<void ()>& fo)
			{
				default_thread_pool()->post(fo);
			}
		};
	}
}

#endif
