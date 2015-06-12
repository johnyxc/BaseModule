#ifndef __AUTO_PTR_HPP_2015_05_26__
#define __AUTO_PTR_HPP_2015_05_26__
#include <bio.hpp>
#include <repeat.hpp>
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		//	只能针对原始指针
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
		//////////////////////////////////////////////////////////////////////////

#define BIT_IMPL(T) \
	public : \
		auto_ptr() { pwt_ = new ptr_wrapper_t<T>; } \
		auto_ptr(T* o) : pwt_() { pwt_ = new ptr_wrapper_t<T>(o); } \
		auto_ptr(ptr_wrapper_t<T>* pwt) { pwt_ = pwt; pwt_->retain(); } \
		~auto_ptr() { if(pwt_) pwt_->release(); } \
		auto_ptr(const auto_ptr& ap) { pwt_ = ap.pwt_; pwt_->retain(); } \
		auto_ptr& operator = (const auto_ptr& ap) { pwt_ = ap.pwt_; pwt_->retain(); return *this; } \
	public : \
		T* operator -> () { if(pwt_) return pwt_->get_ptr(); return 0; } \
		T operator * () { if(pwt_) return *pwt_->get_ptr(); return T(); } \
		T* raw_ptr() { if(pwt_) return pwt_->get_ptr(); return 0; } \
		ptr_wrapper_t<T>* wrapper_ptr() { return pwt_; } \
		bool valid() { return pwt_->get_ptr() != 0; } \
		operator bool () { return valid(); } \
	private : \
		ptr_wrapper_t<T>* pwt_;

		//////////////////////////////////////////////////////////////////////////

		template <typename T>
		struct auto_ptr
		{
			//	智能指针转型函数，一般用于向上转型
			template <typename TS>
			static detail::auto_ptr<T> auto_ptr_dynamic_cast(detail::auto_ptr<TS> psrc)
			{
				auto_ptr<T> pdst((T*)psrc.raw_ptr());
				pdst->retain();
				return pdst;
			}

		public :
			auto_ptr() : pwt_() {}
			auto_ptr(T* o) : pwt_(o) {}
			auto_ptr(const auto_ptr& ap)
			{
				if(!ap.pwt_) return;
				pwt_ = ap.pwt_;
				pwt_->retain();
			}
			~auto_ptr()
			{
				if(pwt_)
					pwt_->release();
			}
			auto_ptr& operator = (const auto_ptr& ap)
			{
				if(!ap.pwt_) return *this;
				pwt_ = ap.pwt_;
				pwt_->retain();
				return *this;
			}

		public :
			T* operator -> () { if(pwt_) return pwt_; return 0; }
			T* raw_ptr() { if(pwt_) return pwt_; return 0; }
			bool valid() { return pwt_ != 0; }
			operator bool () { return valid(); }

		private :
			T* pwt_;
		};

		template <>
		struct auto_ptr<int> { BIT_IMPL(int) };
		template <>
		struct auto_ptr<unsigned int> { BIT_IMPL(unsigned int) };

		template <>
		struct auto_ptr<char> { BIT_IMPL(char) };
		template <>
		struct auto_ptr<unsigned char> { BIT_IMPL(unsigned char) };

		template <>
		struct auto_ptr<long> { BIT_IMPL(long) };
		template <>
		struct auto_ptr<unsigned long> { BIT_IMPL(unsigned long) };

		template <>
		struct auto_ptr<long long> { BIT_IMPL(long long) };
		template <>
		struct auto_ptr<unsigned long long> { BIT_IMPL(unsigned long long) };

		template <>
		struct auto_ptr<float> { BIT_IMPL(float) };
		template <>
		struct auto_ptr<double> { BIT_IMPL(double) };
	}

	//	一般在传实参时使用
	template <typename T>
	detail::auto_ptr<T> retain(T* t)
	{
		detail::auto_ptr<T> ap(t);
		ap->retain();
		return ap;
	}

	//	避免外部直接调用new
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
