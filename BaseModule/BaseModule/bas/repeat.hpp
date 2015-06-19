#ifndef __REPEAT_HPP_2015_05_15__
#define __REPEAT_HPP_2015_05_15__
//	定义一些方便的宏扩展

//////////////////////////////////////////////////////////////////////////
//	自定义类型

//	逗号扩展
#define comma_repeat_0(mcr)
#define comma_repeat_1(mcr)		mcr(1)
#define comma_repeat_2(mcr)		comma_repeat_1(mcr) ,mcr(2)
#define comma_repeat_3(mcr)		comma_repeat_2(mcr) ,mcr(3)
#define comma_repeat_4(mcr)		comma_repeat_3(mcr) ,mcr(4)
#define comma_repeat_5(mcr)		comma_repeat_4(mcr) ,mcr(5)
#define comma_repeat_6(mcr)		comma_repeat_5(mcr) ,mcr(6)
#define comma_repeat_7(mcr)		comma_repeat_6(mcr) ,mcr(7)
#define comma_repeat_8(mcr)		comma_repeat_7(mcr) ,mcr(8)
#define comma_repeat_9(mcr)		comma_repeat_8(mcr) ,mcr(9)
#define comma_repeat(e)			comma_repeat_##e

//	分号扩展
#define semic_repeat_0(mcr)
#define semic_repeat_1(mcr)		mcr(1)
#define semic_repeat_2(mcr)		semic_repeat_1(mcr) ;mcr(2)
#define semic_repeat_3(mcr)		semic_repeat_2(mcr) ;mcr(3)
#define semic_repeat_4(mcr)		semic_repeat_3(mcr) ;mcr(4)
#define semic_repeat_5(mcr)		semic_repeat_4(mcr) ;mcr(5)
#define semic_repeat_6(mcr)		semic_repeat_5(mcr) ;mcr(6)
#define semic_repeat_7(mcr)		semic_repeat_6(mcr) ;mcr(7)
#define semic_repeat_8(mcr)		semic_repeat_7(mcr) ;mcr(8)
#define semic_repeat_9(mcr)		semic_repeat_8(mcr) ;mcr(9)
#define semic_repeat(e)			semic_repeat_##e

//	空格扩展
#define blank_repeat_0(mcr)
#define blank_repeat_1(mcr)		mcr(1)
#define blank_repeat_2(mcr)		blank_repeat_1(mcr) mcr(2)
#define blank_repeat_3(mcr)		blank_repeat_2(mcr) mcr(3)
#define blank_repeat_4(mcr)		blank_repeat_3(mcr) mcr(4)
#define blank_repeat_5(mcr)		blank_repeat_4(mcr) mcr(5)
#define blank_repeat_6(mcr)		blank_repeat_5(mcr) mcr(6)
#define blank_repeat_7(mcr)		blank_repeat_6(mcr) mcr(7)
#define blank_repeat_8(mcr)		blank_repeat_7(mcr) mcr(8)
#define blank_repeat_9(mcr)		blank_repeat_8(mcr) mcr(9)
#define blank_repeat(e)			blank_repeat_##e

//	封装
#define comma_expand(mcr, e)	comma_repeat(e)(mcr)
#define semic_expand(mcr, e)	semic_repeat(e)(mcr)
#define blank_expand(mcr, e)	blank_repeat(e)(mcr)

//	常用定义1
#define exp_template_list(i)				typename P##i##_##i
#define	exp_class_list(i)					class P##i##_##i
#define	exp_formal_list(i)					P##i##_##i p##i##_##i
#define	exp_type_list(i)					P##i##_##i
#define	exp_actual_list(i)					p##i##_##i
#define	exp_formal_refer_list(i)			P##i##_##i & p##i##_##i
#define	exp_formal_const_list(i)			P##i##_##i const p##i##_##i
#define	exp_formal_const_refer_list(i)		P##i##_##i const & p##i##_##i

//	常用定义2
#define exp_template_list_ph(i)				typename PH##i##_##i
#define	exp_class_list_ph(i)				class PH##i##_##i
#define	exp_formal_list_ph(i)				PH##i##_##i ph##i##_##i
#define	exp_type_list_ph(i)					PH##i##_##i
#define	exp_actual_list_ph(i)				ph##i##_##i
#define	exp_formal_refer_list_ph(i)			PH##i##_##i & ph##i##_##i
#define	exp_formal_const_list_ph(i)			PH##i##_##i const ph##i##_##i
#define	exp_formal_const_refer_list_ph(i)	PH##i##_##i const & ph##i##_##i

#endif
