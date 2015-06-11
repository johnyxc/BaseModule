#ifndef __AUTO_PTR_HPP_2015_05_26__
#define __AUTO_PTR_HPP_2015_05_26__
#include <bio.hpp>
#include <repeat.hpp>
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		//	ֻ�����ԭʼָ��
		template <typename T>
		struct ptr_wrapper_t : bio_bas_t<ptr_wrapper_t<T> >
		{
			template <typename T>
			friend struct auto_ptr;

		public :
			ptr_wrapper_t() : o_() {}
			ptr_wrapper_t(T* o) : o_(o) {}
			~ptr_wrapper_t() { if(o_) delete o_; }
			ptr_wrapper_t(const ptr_wrapper_t& pwt) { o_ = pwt.o_; }
			ptr_wrapper_t& operator = (const ptr_wrapper_t& pwt) { o_ = pwt.o_; return *this; }

		public :
			T* get_ptr() { return o_; }

		private :
			T* o_;
		};

		template <typename T>
		struct auto_ptr
		{
			//	����ָ��ת�ͺ�����һ����������ת��
			template <typename TS>
			static detail::auto_ptr<T> auto_ptr_dynamic_cast(detail::auto_ptr<TS> psrc)
			{
				auto_ptr<T> pdst((ptr_wrapper_t<T>*)psrc.wrapper_ptr());
				return pdst;
			}

		public :
			auto_ptr() { pwt_ = new ptr_wrapper_t<T>; }
			auto_ptr(T* o) : pwt_() { pwt_ = new ptr_wrapper_t<T>(o); }
			auto_ptr(ptr_wrapper_t<T>* pwt) { pwt_ = pwt; pwt_->retain(); }
			~auto_ptr() { if(pwt_) pwt_->release(); }
			auto_ptr(const auto_ptr& ap) { pwt_ = ap.pwt_; pwt_->retain(); }
			auto_ptr& operator = (const auto_ptr& ap) { pwt_ = ap.pwt_; pwt_->retain(); return *this; }

		public :
			T* operator -> () { if(pwt_) return pwt_->get_ptr(); return 0; }
			T operator * () { if(pwt_) return *pwt_->get_ptr(); return T(); }
			T* raw_ptr() { if(pwt_) return pwt_->get_ptr(); return 0; }
			ptr_wrapper_t<T>* wrapper_ptr() { return pwt_; }
			bool valid() { return pwt_->get_ptr() != 0; }
			operator bool () { return valid(); }

		private :
			ptr_wrapper_t<T>* pwt_;
		};
	}

	//	һ���ڴ�ʵ��ʱʹ��
	template <typename T>
	detail::auto_ptr<T> retain(T* t)
	{
		return detail::auto_ptr<T>(t);
	}

	//	�����ⲿֱ�ӵ���new
	template <typename T>
	detail::auto_ptr<T> make_auto_ptr()
	{
		return detail::auto_ptr<T>(new T);
	}

#define MAKE_AUTO_PTR_EXPAND(i) \
	template <typename T, comma_expand(exp_template_list, i)> \
	detail::auto_ptr<T> make_auto_ptr(comma_expand(exp_formal_list, i)) \
	{ \
		return detail::auto_ptr<T>(new T(comma_expand(exp_actual_list, i))); \
	}

	blank_expand(MAKE_AUTO_PTR_EXPAND, 9)
}

#endif
