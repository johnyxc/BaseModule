#ifndef __STRAND_HPP_2015_06_08__
#define __STRAND_HPP_2015_06_08__
#include <bio.hpp>
#include <function.hpp>

namespace bas
{
	namespace detail
	{
		struct strand_t : bas::detail::bio_bas_t<strand_t>
		{
		public :
			strand_t() {}
			~strand_t() {}
		};
	}
}

#endif
