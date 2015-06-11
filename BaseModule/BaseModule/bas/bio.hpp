#ifndef __BIO_HPP_2015_06_03__
#define __BIO_HPP_2015_06_03__
#include <windows.h>
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
			void retain() { InterlockedIncrement(&ref_); }

			void release()
			{
				InterlockedDecrement(&ref_);
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
