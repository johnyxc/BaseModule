#ifndef __BIO_HPP_2015_06_03__
#define __BIO_HPP_2015_06_03__
#include <osfunc.hpp>
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		template <typename T>
		struct bio_bas_t
		{
		public :
			bio_bas_t() : ref_(1) {}
			~bio_bas_t() {}

		public :
			void retain() { atom_inc(&ref_); }

			void release()
			{
				atom_sub(&ref_);
				if(ref_ == 0)
				{
					T* obj = static_cast<T*>(this);
					delete obj;
				}
			}

		private :
			long ref_;
		};
	}
}

#endif
