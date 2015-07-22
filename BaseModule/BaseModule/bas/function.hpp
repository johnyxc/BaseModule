#ifndef __FUNCTION_HPP_2015_05_25__
#define __FUNCTION_HPP_2015_05_25__
#include <placeholder.hpp>
#include <auto_ptr.hpp>
#include <repeat.hpp>
#include <map>
//////////////////////////////////////////////////////////////////////////

namespace bas
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		//	只提供唯一版本
		//	所有其他版本可以通过实例化获得
#define VFUN_EXPAND(i) \
	virtual RT operator ()(comma_expand(exp_type_list, i)) { return RT(); }

		template <typename RT, typename P1_1 = int, typename P2_2 = int, typename P3_3 = int,
			typename P4_4 = int, typename P5_5 = int, typename P6_6 = int,
			typename P7_7 = int, typename P8_8 = int, typename P9_9 = int>
		struct function_wrapper : bio_bas_t<function_wrapper<RT,
			P1_1, P2_2, P3_3, P4_4,
			P5_5, P6_6, P7_7, P8_8, P9_9> >
		{
			function_wrapper() {}
			virtual ~function_wrapper() {}
			virtual RT operator ()() { return RT(); }
			blank_expand(VFUN_EXPAND, 9)
		};

		//////////////////////////////////////////////////////////////////////////
		//	无参普通C函数版本
		template <typename RT>
		struct function_cstyle_wrapper : function_wrapper<RT>
		{
			typedef RT(*FUN)();

		public :
			function_cstyle_wrapper() : fun_() {}
			function_cstyle_wrapper(const function_cstyle_wrapper& fo) { fun_ = fo.fun_; }
			~function_cstyle_wrapper() { fun_ = 0; }
			function_cstyle_wrapper& operator = (const function_cstyle_wrapper& fo) { fun_ = fo.fun_; return *this; }

		public :
			virtual RT operator ()() { if(fun_) return fun_(); return RT(); }
			virtual RT operator ()(int) { return RT(); }
			virtual RT operator ()(int, int) { return RT(); }
			virtual RT operator ()(int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
		};

		//	无参成员函数版本
		template <typename CT, typename RT>
		struct function_mem_wrapper : function_wrapper<RT>
		{
			typedef RT(CT::*CFUN)();

		public :
			function_mem_wrapper() : fun_(), self_() {}
			function_mem_wrapper(const function_mem_wrapper& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; }
			~function_mem_wrapper() { fun_ = 0; self_ = 0; }
			function_mem_wrapper& operator = (const function_mem_wrapper& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; return *this; }

		public :
			virtual RT operator ()() { if(self_&&fun_) return (self_->*fun_)(); return RT(); }
			virtual RT operator ()(int) { return RT(); }
			virtual RT operator ()(int, int) { return RT(); }
			virtual RT operator ()(int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(int, int, int, int, int, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	1参数普通C函数版本
		template <typename RT, typename P1, typename PH1>
		struct function_cstyle_wrapper_1 : function_wrapper<RT, P1>
		{
			typedef RT(*FUN)(P1);

		public :
			function_cstyle_wrapper_1(const CallList1<PH1>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_1(const function_cstyle_wrapper_1& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_1() { fun_ = 0; }
			function_cstyle_wrapper_1& operator = (const function_cstyle_wrapper_1& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				PH1 ph1 = cl_[cl_.p1_];
				if(fun_) return fun_(*(P1*)(void*)(&ph1));
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_]);
				return RT();
			}
			RT operator ()(P1, int) { return RT(); }
			virtual RT operator ()(P1, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList1<PH1> cl_;
			RealList1<P1> rl_;
		};

		//	1参数成员函数版本
		template <typename CT, typename RT, typename P1, typename PH1>
		struct function_mem_wrapper_1 : function_wrapper<RT, P1>
		{
			typedef RT(CT::*CFUN)(P1);

		public :
			function_mem_wrapper_1(const CallList1<PH1>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_1(const function_mem_wrapper_1& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_1() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_1& operator = (const function_mem_wrapper_1& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(self_&&fun_) return (self_->*fun_)(rl_[cl_.p1_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(self_&&fun_) return (self_->*fun_)(rl_[cl_.p1_]);
				return RT();
			}
			virtual RT operator ()(P1, int) { return RT(); }
			virtual RT operator ()(P1, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, int, int, int, int, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList1<PH1> cl_;
			RealList1<P1> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	2参数普通C函数版本
		template <typename RT, typename P1, typename P2,
			typename PH1, typename PH2>
		struct function_cstyle_wrapper_2 : function_wrapper<RT, P1, P2>
		{
			typedef RT(*FUN)(P1, P2);

		public :
			function_cstyle_wrapper_2(const CallList2<PH1, PH2>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_2(const function_cstyle_wrapper_2& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_2() { fun_ = 0; }
			function_cstyle_wrapper_2& operator = (const function_cstyle_wrapper_2& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList2<PH1, PH2> cl_;
			RealList2<P1, P2> rl_;
		};

		//	2参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2,
			typename PH1, typename PH2>
		struct function_mem_wrapper_2 : function_wrapper<RT, P1, P2>
		{
			typedef RT(CT::*CFUN)(P1, P2);

		public :
			function_mem_wrapper_2(const CallList2<PH1, PH2>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_2(const function_mem_wrapper_2& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_2() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_2& operator = (const function_mem_wrapper_2& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, int, int, int, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList2<PH1, PH2> cl_;
			RealList2<P1, P2> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	3参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3,
			typename PH1, typename PH2, typename PH3>
		struct function_cstyle_wrapper_3 : function_wrapper<RT, P1, P2, P3>
		{
			typedef RT(*FUN)(P1, P2, P3);

		public :
			function_cstyle_wrapper_3(const CallList3<PH1, PH2, PH3>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_3(const function_cstyle_wrapper_3& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_3() { fun_ = 0; }
			function_cstyle_wrapper_3& operator = (const function_cstyle_wrapper_3& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList3<PH1, PH2, PH3> cl_;
			RealList3<P1, P2, P3> rl_;
		};

		//	3参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3,
			typename PH1, typename PH2, typename PH3>
		struct function_mem_wrapper_3 : function_wrapper<RT, P1, P2, P3>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3);

		public :
			function_mem_wrapper_3(const CallList3<PH1, PH2, PH3>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_3(const function_mem_wrapper_3& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_3() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_3& operator = (const function_mem_wrapper_3& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, int, int, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList3<PH1, PH2, PH3> cl_;
			RealList3<P1, P2, P3> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	4参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename PH1, typename PH2, typename PH3, typename PH4>
		struct function_cstyle_wrapper_4 : function_wrapper<RT, P1, P2, P3, P4>
		{
			typedef RT(*FUN)(P1, P2, P3, P4);

		public :
			function_cstyle_wrapper_4(const CallList4<PH1, PH2, PH3, PH4>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_4(const function_cstyle_wrapper_4& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_4() { fun_ = 0; }
			function_cstyle_wrapper_4& operator = (const function_cstyle_wrapper_4& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList4<PH1, PH2, PH3, PH4> cl_;
			RealList4<P1, P2, P3, P4> rl_;
		};

		//	4参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4,
			typename PH1, typename PH2, typename PH3, typename PH4>
		struct function_mem_wrapper_4 : function_wrapper<RT, P1, P2, P3, P4>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4);

		public :
			function_mem_wrapper_4(const CallList4<PH1, PH2, PH3, PH4>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_4(const function_mem_wrapper_4& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_4() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_4& operator = (const function_mem_wrapper_4& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, int, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList4<PH1, PH2, PH3, PH4> cl_;
			RealList4<P1, P2, P3, P4> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	5参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5>
		struct function_cstyle_wrapper_5 : function_wrapper<RT, P1, P2, P3, P4, P5>
		{
			typedef RT(*FUN)(P1, P2, P3, P4, P5);

		public :
			function_cstyle_wrapper_5(const CallList5<PH1, PH2, PH3, PH4, PH5>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_5(const function_cstyle_wrapper_5& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_5() { fun_ = 0; }
			function_cstyle_wrapper_5& operator = (const function_cstyle_wrapper_5& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList5<PH1, PH2, PH3, PH4, PH5> cl_;
			RealList5<P1, P2, P3, P4, P5> rl_;
		};

		//	5参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5>
		struct function_mem_wrapper_5 : function_wrapper<RT, P1, P2, P3, P4, P5>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4, P5);

		public :
			function_mem_wrapper_5(const CallList5<PH1, PH2, PH3, PH4, PH5>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_5(const function_mem_wrapper_5& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_5() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_5& operator = (const function_mem_wrapper_5& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, int, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList5<PH1, PH2, PH3, PH4, PH5> cl_;
			RealList5<P1, P2, P3, P4, P5> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	6参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6>
		struct function_cstyle_wrapper_6 : function_wrapper<RT, P1, P2, P3, P4, P5, P6>
		{
			typedef RT(*FUN)(P1, P2, P3, P4, P5, P6);

		public :
			function_cstyle_wrapper_6(const CallList6<PH1, PH2, PH3, PH4, PH5, PH6>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_6(const function_cstyle_wrapper_6& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_6() { fun_ = 0; }
			function_cstyle_wrapper_6& operator = (const function_cstyle_wrapper_6& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList6<PH1, PH2, PH3, PH4, PH5, PH6> cl_;
			RealList6<P1, P2, P3, P4, P5, P6> rl_;
		};

		//	6参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6>
		struct function_mem_wrapper_6 : function_wrapper<RT, P1, P2, P3, P4, P5, P6>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4, P5, P6);

		public :
			function_mem_wrapper_6(const CallList6<PH1, PH2, PH3, PH4, PH5, PH6>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_6(const function_mem_wrapper_6& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_6() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_6& operator = (const function_mem_wrapper_6& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, int, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList6<PH1, PH2, PH3, PH4, PH5, PH6> cl_;
			RealList6<P1, P2, P3, P4, P5, P6> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	7参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7>
		struct function_cstyle_wrapper_7 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7>
		{
			typedef RT(*FUN)(P1, P2, P3, P4, P5, P6, P7);

		public :
			function_cstyle_wrapper_7(const CallList7<PH1, PH2, PH3, PH4, PH5, PH6, PH7>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_7(const function_cstyle_wrapper_7& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_7() { fun_ = 0; }
			function_cstyle_wrapper_7& operator = (const function_cstyle_wrapper_7& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, int, int) { return RT(); }

		public :
			FUN fun_;
			CallList7<PH1, PH2, PH3, PH4, PH5, PH6, PH7> cl_;
			RealList7<P1, P2, P3, P4, P5, P6, P7> rl_;
		};

		//	7参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7>
		struct function_mem_wrapper_7 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4, P5, P6, P7);

		public :
			function_mem_wrapper_7(const CallList7<PH1, PH2, PH3, PH4, PH5, PH6, PH7>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_7(const function_mem_wrapper_7& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_7() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_7& operator = (const function_mem_wrapper_7& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, int) { return RT(); }
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, int, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList7<PH1, PH2, PH3, PH4, PH5, PH6, PH7> cl_;
			RealList7<P1, P2, P3, P4, P5, P6, P7> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	8参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7, typename PH8>
		struct function_cstyle_wrapper_8 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8>
		{
			typedef RT(*FUN)(P1, P2, P3, P4, P5, P6, P7, P8);

		public :
			function_cstyle_wrapper_8(const CallList8<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_8(const function_cstyle_wrapper_8& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_8() { fun_ = 0; }
			function_cstyle_wrapper_8& operator = (const function_cstyle_wrapper_8& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, P8, int) { return RT(); }

		public :
			FUN fun_;
			CallList8<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8> cl_;
			RealList8<P1, P2, P3, P4, P5, P6, P7, P8> rl_;
		};

		//	8参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7, typename PH8>
		struct function_mem_wrapper_8 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4, P5, P6, P7, P8);

		public :
			function_mem_wrapper_8(const CallList8<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_8(const function_mem_wrapper_8& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_8() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_8& operator = (const function_mem_wrapper_8& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_]);
				return RT();
			}
			virtual RT operator ()(P1, P2, P3, P4, P5, P6, P7, P8, int) { return RT(); }

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList8<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8> cl_;
			RealList8<P1, P2, P3, P4, P5, P6, P7, P8> rl_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	9参数普通C函数版本
		template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7, typename PH8, typename PH9>
		struct function_cstyle_wrapper_9 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8, P9>
		{
			typedef RT(*FUN)(P1, P2, P3, P4, P5, P6, P7, P8, P9);

		public :
			function_cstyle_wrapper_9(const CallList9<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9>& cl) : fun_(), cl_(cl) {}
			function_cstyle_wrapper_9(const function_cstyle_wrapper_9& fo) { fun_ = fo.fun_; cl_ = fo.cl_; }
			~function_cstyle_wrapper_9() { fun_ = 0; }
			function_cstyle_wrapper_9& operator = (const function_cstyle_wrapper_9& fo) { fun_ = fo.fun_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				rl_.p9_ = p9;
				if(fun_) return fun_(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}

		public :
			FUN fun_;
			CallList9<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9> cl_;
			RealList9<P1, P2, P3, P4, P5, P6, P7, P8, P9> rl_;
		};

		//	9参数成员函数版本
		template <typename CT, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9,
			typename PH1, typename PH2, typename PH3, typename PH4, typename PH5, typename PH6, typename PH7, typename PH8, typename PH9>
		struct function_mem_wrapper_9 : function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8, P9>
		{
			typedef RT(CT::*CFUN)(P1, P2, P3, P4, P5, P6, P7, P8, P9);

		public :
			function_mem_wrapper_9(const CallList9<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9>& cl) : fun_(), self_(), cl_(cl) {}
			function_mem_wrapper_9(const function_mem_wrapper_9& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; }
			~function_mem_wrapper_9() { fun_ = 0; self_ = 0; }
			function_mem_wrapper_9& operator = (const function_mem_wrapper_9& fo) { fun_ = fo.fun_; self_ = fo.self_; auto_self_ = fo.auto_self_; cl_ = fo.cl_; return *this; }

		public :
			virtual RT operator ()() {
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}

			virtual RT operator ()(P1 p1) {
				rl_.p1_ = p1;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}

			virtual RT operator ()(P1 p1, P2 p2) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}
			virtual RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) {
				rl_.p1_ = p1;
				rl_.p2_ = p2;
				rl_.p3_ = p3;
				rl_.p4_ = p4;
				rl_.p5_ = p5;
				rl_.p6_ = p6;
				rl_.p7_ = p7;
				rl_.p8_ = p8;
				rl_.p9_ = p9;
				if(fun_&&self_) return (self_->*fun_)(rl_[cl_.p1_], rl_[cl_.p2_], rl_[cl_.p3_], rl_[cl_.p4_],
					rl_[cl_.p5_], rl_[cl_.p6_], rl_[cl_.p7_], rl_[cl_.p8_], rl_[cl_.p9_]);
				return RT();
			}

		public :
			CFUN fun_;
			CT* self_;
			auto_ptr<CT> auto_self_;
			CallList9<PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9> cl_;
			RealList9<P1, P2, P3, P4, P5, P6, P7, P8, P9> rl_;
		};

		//	赋值构造扩展模板
#define ASSIGN_EXPAND(i) \
	template <comma_expand(exp_template_list, i)> \
	function(const function<RT(comma_expand(exp_type_list, i))>& fo) { invoker_ = auto_ptr<PROPERTY_TYPE >::auto_ptr_dynamic_cast(fo.invoker_); }

		//////////////////////////////////////////////////////////////////////////
		//	外部使用的最终形式
		template <typename SIG>
		struct function {};

		template <typename RT>
		struct function<RT()>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()() { if(invoker_) return invoker_->operator()(); return RT(); }

		public :
			auto_ptr<function_wrapper<RT> > invoker_;
		};

		template <typename RT, typename P1>
		struct function<RT(P1)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1) { if(invoker_) return invoker_->operator()(p1); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1> > invoker_;
		};

		template <typename RT, typename P1, typename P2>
		struct function<RT(P1, P2)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2) { if(invoker_) return invoker_->operator()(p1, p2); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3>
		struct function<RT(P1, P2, P3)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3) { if(invoker_) return invoker_->operator()(p1, p2, p3); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4>
		struct function<RT(P1, P2, P3, P4)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename P5>
		struct function<RT(P1, P2, P3, P4, P5)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4, P5>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4,
				p5); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4, P5> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6>
		struct function<RT(P1, P2, P3, P4, P5, P6)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4, P5, P6>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4,
				p5, p6); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4, P5, P6> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7>
		struct function<RT(P1, P2, P3, P4, P5, P6, P7)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4,
				p5, p6, p7); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8>
		struct function<RT(P1, P2, P3, P4, P5, P6, P7, P8)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4,
				p5, p6, p7, p8); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8> > invoker_;
		};

		template <typename RT, typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9>
		struct function<RT(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
		{
		public :
			function() : invoker_() {}
			function(const function& fo) { invoker_ = fo.invoker_; }
#define PROPERTY_TYPE function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8, P9>
			blank_expand(ASSIGN_EXPAND, 9)
#undef PROPERTY_TYPE
			~function() {}
			function& operator = (const function& fo) { invoker_ = fo.invoker_; return *this; }
			bool valid() { return invoker_.valid(); }

		public :
			RT operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) { if(invoker_) return invoker_->operator()(p1, p2, p3, p4,
				p5, p6, p7, p8, p9); return RT(); }

		public :
			auto_ptr<function_wrapper<RT, P1, P2, P3, P4, P5, P6, P7, P8, P9> > invoker_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	信号
		template <typename SIG>
		struct signal {};

		template <typename RT>
		struct signal<RT()>
		{
		public :
			signal() : key_() {}
			~signal() {}

		public :
			int slot(function<RT()> fo)
			{
				int key = generate_key();
				fo_list_.insert(std::pair<int, function<RT()> >(key, fo));
				return key;
			}

			void cancel(int key)
			{
				typename std::map<int, function<RT()> >::iterator iter;
				iter = fo_list_.find(key);
				if(iter == fo_list_.end()) return;
				fo_list_.erase(iter);
			}

			RT operator ()()
			{
				typename std::map<int, function<RT()> >::iterator iter;
				for(iter = fo_list_.begin(); iter != fo_list_.end(); ++iter)
				{
					(iter->second)();
				}
			}

		private :
			int generate_key()
			{
				return ++key_;
			}

		private :
			std::map<int, function<RT()> > fo_list_;
			int key_;
		};

#define SIGNAL_EXPAND(i) \
		template <typename RT, comma_expand(exp_template_list, i)> \
		struct signal<RT(comma_expand(exp_type_list, i))> \
		{ \
		public : \
			signal() : key_() {} \
			~signal() {} \
\
		public : \
			int slot(function<RT(comma_expand(exp_type_list, i))> fo) \
			{ \
				int key = generate_key(); \
				fo_list_.insert(std::pair<int, function<RT(comma_expand(exp_type_list, i))> >(key, fo)); \
				return key; \
			} \
\
			void cancel(int key) \
			{ \
				typename std::map<int, function<RT(comma_expand(exp_type_list, i))> >::iterator iter; \
				iter = fo_list_.find(key); \
				if(iter == fo_list_.end()) return; \
				fo_list_.erase(iter); \
			} \
\
			RT operator ()(comma_expand(exp_formal_list, i)) \
			{ \
				typename std::map<int, function<RT(comma_expand(exp_type_list, i))> >::iterator iter; \
				for(iter = fo_list_.begin(); iter != fo_list_.end(); ++iter) \
				{ \
					(iter->second)(comma_expand(exp_actual_list, i)); \
				} \
			} \
\
		private : \
			int generate_key() \
			{ \
				return ++key_; \
			} \
\
		private : \
			std::map<int, function<RT(comma_expand(exp_type_list, i))> > fo_list_; \
			int key_; \
		};

		blank_expand(SIGNAL_EXPAND, 9)
	}
}

#endif
