#ifndef __TSTORAGE_HPP_2015_05_26__
#define __TSTORAGE_HPP_2015_05_26__
#include <repeat.hpp>

namespace bas
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		//	place holder

		struct place_holder_t1 {};
		struct place_holder_t2 {};
		struct place_holder_t3 {};
		struct place_holder_t4 {};
		struct place_holder_t5 {};
		struct place_holder_t6 {};
		struct place_holder_t7 {};
		struct place_holder_t8 {};
		struct place_holder_t9 {};

		static place_holder_t1 _1;
		static place_holder_t2 _2;
		static place_holder_t3 _3;
		static place_holder_t4 _4;
		static place_holder_t5 _5;
		static place_holder_t6 _6;
		static place_holder_t7 _7;
		static place_holder_t8 _8;
		static place_holder_t9 _9;

		//////////////////////////////////////////////////////////////////////////
		//	param storage

		struct ArgStorage0
		{};

		template <typename P1>
		struct ArgStorage1 : ArgStorage0
		{
			ArgStorage1(P1 p) : p1_(p) {}
			ArgStorage1(const ArgStorage1& ao) { p1_ = ao.p1_; }
			ArgStorage1& operator = (const ArgStorage1& ao) { p1_ = ao.p1_; return *this; }
			P1 p1_;
		};

		template <typename P1, typename P2>
		struct ArgStorage2 : ArgStorage1<P1>
		{
			ArgStorage2(P1 p1, P2 p2) : ArgStorage1(p1), p2_(p2) {}
			ArgStorage2(const ArgStorage2& ao) : ArgStorage1(ao.p1_) { p2_ = ao.p2_; }
			ArgStorage2& operator = (const ArgStorage2& ao) { p1_ = ao.p1_; p2_ = ao.p2_; return *this; }
			P2 p2_;
		};

		template <typename P1, typename P2, typename P3>
		struct ArgStorage3 : ArgStorage2<P1, P2>
		{
			ArgStorage3(P1 p1, P2 p2, P3 p3) : ArgStorage2(p1, p2), p3_(p3) {}
			ArgStorage3(const ArgStorage3& ao) : ArgStorage2(ao.p1_, ao.p2_) { p3_ = ao.p3_; }
			ArgStorage3& operator = (const ArgStorage3& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; return *this; }
			P3 p3_;
		};

		template <typename P1, typename P2, typename P3, typename P4>
		struct ArgStorage4 : ArgStorage3<P1, P2, P3>
		{
			ArgStorage4(P1 p1, P2 p2, P3 p3, P4 p4) : ArgStorage3(p1, p2, p3), p4_(p4) {}
			ArgStorage4(const ArgStorage4& ao) : ArgStorage3(ao.p1_, ao.p2_, ao.p3_) { p4_ = ao.p4_; }
			ArgStorage4& operator = (const ArgStorage4& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; return *this; }
			P4 p4_;
		};

		template <typename P1, typename P2, typename P3, typename P4, typename P5>
		struct ArgStorage5 : ArgStorage4<P1, P2, P3, P4>
		{
			ArgStorage5(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) : ArgStorage4(p1, p2, p3, p4), p5_(p5) {}
			ArgStorage5(const ArgStorage5& ao) : ArgStorage4(ao.p1_, ao.p2_, ao.p3_, ao.p4_) { p5_ = ao.p5_; }
			ArgStorage5& operator = (const ArgStorage5& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; p5_ = ao.p5_; return *this; }
			P5 p5_;
		};

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		struct ArgStorage6 : ArgStorage5<P1, P2, P3, P4, P5>
		{
			ArgStorage6(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) : ArgStorage5(p1, p2, p3, p4, p5), p6_(p6) {}
			ArgStorage6(const ArgStorage6& ao) : ArgStorage5(ao.p1_, ao.p2_, ao.p3_, ao.p4_, ao.p5_) { p6_ = ao.p6_; }
			ArgStorage6& operator = (const ArgStorage6& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; p5_ = ao.p5_; p6_ = ao.p6_; return *this; }
			P6 p6_;
		};

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		struct ArgStorage7 : ArgStorage6<P1, P2, P3, P4, P5, P6>
		{
			ArgStorage7(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) : ArgStorage6(p1, p2, p3, p4, p5, p6), p7_(p7) {}
			ArgStorage7(const ArgStorage7& ao) : ArgStorage6(ao.p1_, ao.p2_, ao.p3_, ao.p4_, ao.p5_, ao.p6_) { p7_ = ao.p7_; }
			ArgStorage7& operator = (const ArgStorage7& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; p5_ = ao.p5_; p6_ = ao.p6_; p7_ = ao.p7_; return *this; }
			P7 p7_;
		};

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		struct ArgStorage8 : ArgStorage7<P1, P2, P3, P4, P5, P6, P7>
		{
			ArgStorage8(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) : ArgStorage7(p1, p2, p3, p4, p5, p6, p7), p8_(p8) {}
			ArgStorage8(const ArgStorage8& ao) : ArgStorage7(ao.p1_, ao.p2_, ao.p3_, ao.p4_, ao.p5_, ao.p6_, ao.p7_) { p8_ = ao.p8_; }
			ArgStorage8& operator = (const ArgStorage8& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; p5_ = ao.p5_; p6_ = ao.p6_; p7_ = ao.p7_; p8_ = ao.p8_; return *this; }
			P8 p8_;
		};

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		struct ArgStorage9 : ArgStorage8<P1, P2, P3, P4, P5, P6, P7, P8>
		{
			ArgStorage9(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) : ArgStorage8(p1, p2, p3, p4, p5, p6, p7, p8), p9_(p9) {}
			ArgStorage9(const ArgStorage9& ao) : ArgStorage8(ao.p1_, ao.p2_, ao.p3_, ao.p4_, ao.p5_, ao.p6_, ao.p7_, ao.p8_) { p9_ = ao.p9_; }
			ArgStorage9& operator = (const ArgStorage9& ao) { p1_ = ao.p1_; p2_ = ao.p2_; p3_ = ao.p3_; p4_ = ao.p4_; p5_ = ao.p5_; p6_ = ao.p6_; p7_ = ao.p7_; p8_ = ao.p8_; p9_ = ao.p9_; return *this; }
			P9 p9_;
		};

		//////////////////////////////////////////////////////////////////////////
		//	call list

		struct CallList0
		{};

		template <typename P1>
		struct CallList1 : ArgStorage1<P1>
		{
			CallList1(P1 p) : ArgStorage1(p) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 2)>
		struct CallList2 : ArgStorage2<comma_expand(exp_type_list, 2)>
		{
			CallList2(comma_expand(exp_formal_list, 2)) : ArgStorage2(comma_expand(exp_actual_list, 2)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 3)>
		struct CallList3 : ArgStorage3<comma_expand(exp_type_list, 3)>
		{
			CallList3(comma_expand(exp_formal_list, 3)) : ArgStorage3(comma_expand(exp_actual_list, 3)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 4)>
		struct CallList4 : ArgStorage4<comma_expand(exp_type_list, 4)>
		{
			CallList4(comma_expand(exp_formal_list, 4)) : ArgStorage4(comma_expand(exp_actual_list, 4)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 5)>
		struct CallList5 : ArgStorage5<comma_expand(exp_type_list, 5)>
		{
			CallList5(comma_expand(exp_formal_list, 5)) : ArgStorage5(comma_expand(exp_actual_list, 5)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 6)>
		struct CallList6 : ArgStorage6<comma_expand(exp_type_list, 6)>
		{
			CallList6(comma_expand(exp_formal_list, 6)) : ArgStorage6(comma_expand(exp_actual_list, 6)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 7)>
		struct CallList7 : ArgStorage7<comma_expand(exp_type_list, 7)>
		{
			CallList7(comma_expand(exp_formal_list, 7)) : ArgStorage7(comma_expand(exp_actual_list, 7)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 8)>
		struct CallList8 : ArgStorage8<comma_expand(exp_type_list, 8)>
		{
			CallList8(comma_expand(exp_formal_list, 8)) : ArgStorage8(comma_expand(exp_actual_list, 8)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		template <comma_expand(exp_template_list, 9)>
		struct CallList9 : ArgStorage9<comma_expand(exp_type_list, 9)>
		{
			CallList9(comma_expand(exp_formal_list, 9)) : ArgStorage9(comma_expand(exp_actual_list, 9)) {}
			template <typename T>
			T operator [] (T t) { return t; }
		};

		//////////////////////////////////////////////////////////////////////////
		//	real list

		struct RealList0
		{};

		template <typename P1>
		struct RealList1
		{
			P1 operator [] (place_holder_t1) { return p1_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1 p1_;
		};

		template <comma_expand(exp_template_list, 2)>
		struct RealList2
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
		};

		template <comma_expand(exp_template_list, 3)>
		struct RealList3
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
		};

		template <comma_expand(exp_template_list, 4)>
		struct RealList4
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
		};

		template <comma_expand(exp_template_list, 5)>
		struct RealList5
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			P5_5 operator [] (place_holder_t5) { return p5_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
			P5_5 p5_;
		};

		template <comma_expand(exp_template_list, 6)>
		struct RealList6
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			P5_5 operator [] (place_holder_t5) { return p5_; }
			P6_6 operator [] (place_holder_t6) { return p6_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
			P5_5 p5_;
			P6_6 p6_;
		};

		template <comma_expand(exp_template_list, 7)>
		struct RealList7
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			P5_5 operator [] (place_holder_t5) { return p5_; }
			P6_6 operator [] (place_holder_t6) { return p6_; }
			P7_7 operator [] (place_holder_t7) { return p7_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
			P5_5 p5_;
			P6_6 p6_;
			P7_7 p7_;
		};

		template <comma_expand(exp_template_list, 8)>
		struct RealList8
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			P5_5 operator [] (place_holder_t5) { return p5_; }
			P6_6 operator [] (place_holder_t6) { return p6_; }
			P7_7 operator [] (place_holder_t7) { return p7_; }
			P8_8 operator [] (place_holder_t8) { return p8_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
			P5_5 p5_;
			P6_6 p6_;
			P7_7 p7_;
			P8_8 p8_;
		};

		template <comma_expand(exp_template_list, 9)>
		struct RealList9
		{
			P1_1 operator [] (place_holder_t1) { return p1_; }
			P2_2 operator [] (place_holder_t2) { return p2_; }
			P3_3 operator [] (place_holder_t3) { return p3_; }
			P4_4 operator [] (place_holder_t4) { return p4_; }
			P5_5 operator [] (place_holder_t5) { return p5_; }
			P6_6 operator [] (place_holder_t6) { return p6_; }
			P7_7 operator [] (place_holder_t7) { return p7_; }
			P8_8 operator [] (place_holder_t8) { return p8_; }
			P9_9 operator [] (place_holder_t9) { return p9_; }
			template <typename T>
			T operator [] (T t) { return t; }
			P1_1 p1_;
			P2_2 p2_;
			P3_3 p3_;
			P4_4 p4_;
			P5_5 p5_;
			P6_6 p6_;
			P7_7 p7_;
			P8_8 p8_;
			P9_9 p9_;
		};
	}
}

#endif
