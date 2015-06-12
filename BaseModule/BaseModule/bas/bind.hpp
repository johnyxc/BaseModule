#ifndef __BIND_HPP_2015_05_25__
#define __BIND_HPP_2015_05_25__

//	实现函数绑定，与function配合使用

#include <bio.hpp>
#include <function.hpp>
#include <repeat.hpp>
#include <auto_ptr.hpp>

namespace bas
{
	namespace detail
	{
		//	无参版本
		//	普通C函数绑定
		template <typename RT>
		function<RT()>
			bind(RT(*fun)())
		{
			function<RT()> fo;
			auto_ptr<function_cstyle_wrapper<RT> > wp = new function_cstyle_wrapper<RT>;
			wp->fun_ = fun;
			fo.invoker_ = auto_ptr<function_wrapper<RT> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT>
		function<RT()>
			bind(RT(CT::*fun)(), CT* self)
		{
			function<RT()> fo;
			auto_ptr<function_mem_wrapper<CT, RT> > wp = new function_mem_wrapper<CT, RT>;
			wp->fun_ = fun;
			wp->self_ = self;
			fo.invoker_ = auto_ptr<function_wrapper<RT> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT>
		function<RT()>
			bind(RT(CT::*fun)(), auto_ptr<CT> self)
		{
			function<RT()> fo;
			auto_ptr<function_mem_wrapper<CT, RT> > wp = new function_mem_wrapper<CT, RT>;
			wp->fun_ = fun;
			wp->self_ = self.raw_ptr();
			wp->auto_self_ = self;
			fo.invoker_ = auto_ptr<function_wrapper<RT> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	1参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 1),
			comma_expand(exp_template_list_ph, 1)>
		function<RT(comma_expand(exp_type_list, 1))>
			bind(RT(*fun)(comma_expand(exp_type_list, 1)), comma_expand(exp_formal_list_ph, 1))
		{
			function<RT(comma_expand(exp_type_list, 1))> fo;
			CallList1<comma_expand(exp_type_list_ph, 1)> cl(comma_expand(exp_actual_list_ph, 1));
			auto_ptr<function_cstyle_wrapper_1<RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)> > wp =
				new function_cstyle_wrapper_1<RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 1)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 1),
			comma_expand(exp_template_list_ph, 1)>
			function<RT(comma_expand(exp_type_list, 1))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 1)), CT* self, comma_expand(exp_formal_list_ph, 1))
		{
			function<RT(comma_expand(exp_type_list, 1))> fo;
			CallList1<comma_expand(exp_type_list_ph, 1)> cl(comma_expand(exp_actual_list_ph, 1));
			auto_ptr<function_mem_wrapper_1<CT, RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)> > wp =
				new function_mem_wrapper_1<CT, RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 1)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 1),
			comma_expand(exp_template_list_ph, 1)>
			function<RT(comma_expand(exp_type_list, 1))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 1)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 1))
		{
			function<RT(comma_expand(exp_type_list, 1))> fo;
			CallList1<comma_expand(exp_type_list_ph, 1)> cl(comma_expand(exp_actual_list_ph, 1));
			auto_ptr<function_mem_wrapper_1<CT, RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)> > wp =
				new function_mem_wrapper_1<CT, RT, comma_expand(exp_type_list, 1), comma_expand(exp_type_list_ph, 1)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 1)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	2参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 2),
			comma_expand(exp_template_list_ph, 2)>
		function<RT(comma_expand(exp_type_list, 2))>
			bind(RT(*fun)(comma_expand(exp_type_list, 2)), comma_expand(exp_formal_list_ph, 2))
		{
			function<RT(comma_expand(exp_type_list, 2))> fo;
			CallList2<comma_expand(exp_type_list_ph, 2)> cl(comma_expand(exp_actual_list_ph, 2));
			auto_ptr<function_cstyle_wrapper_2<RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)> > wp =
				new function_cstyle_wrapper_2<RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 2)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 2),
			comma_expand(exp_template_list_ph, 2)>
		function<RT(comma_expand(exp_type_list, 2))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 2)), CT* self, comma_expand(exp_formal_list_ph, 2))
		{
			function<RT(comma_expand(exp_type_list, 2))> fo;
			CallList2<comma_expand(exp_type_list_ph, 2)> cl(comma_expand(exp_actual_list_ph, 2));
			auto_ptr<function_mem_wrapper_2<CT, RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)> > wp =
				new function_mem_wrapper_2<CT, RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 2)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 2),
			comma_expand(exp_template_list_ph, 2)>
			function<RT(comma_expand(exp_type_list, 2))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 2)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 2))
		{
			function<RT(comma_expand(exp_type_list, 2))> fo;
			CallList2<comma_expand(exp_type_list_ph, 2)> cl(comma_expand(exp_actual_list_ph, 2));
			auto_ptr<function_mem_wrapper_2<CT, RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)> > wp =
				new function_mem_wrapper_2<CT, RT, comma_expand(exp_type_list, 2), comma_expand(exp_type_list_ph, 2)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 2)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	3参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 3),
			comma_expand(exp_template_list_ph, 3)>
		function<RT(comma_expand(exp_type_list, 3))>
			bind(RT(*fun)(comma_expand(exp_type_list, 3)), comma_expand(exp_formal_list_ph, 3))
		{
			function<RT(comma_expand(exp_type_list, 3))> fo;
			CallList3<comma_expand(exp_type_list_ph, 3)> cl(comma_expand(exp_actual_list_ph, 3));
			auto_ptr<function_cstyle_wrapper_3<RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)> > wp =
				new function_cstyle_wrapper_3<RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 3)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 3),
			comma_expand(exp_template_list_ph, 3)>
		function<RT(comma_expand(exp_type_list, 3))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 3)), CT* self, comma_expand(exp_formal_list_ph, 3))
		{
			function<RT(comma_expand(exp_type_list, 3))> fo;
			CallList3<comma_expand(exp_type_list_ph, 3)> cl(comma_expand(exp_actual_list_ph, 3));
			auto_ptr<function_mem_wrapper_3<CT, RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)> > wp =
				new function_mem_wrapper_3<CT, RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 3)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 3),
			comma_expand(exp_template_list_ph, 3)>
			function<RT(comma_expand(exp_type_list, 3))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 3)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 3))
		{
			function<RT(comma_expand(exp_type_list, 3))> fo;
			CallList3<comma_expand(exp_type_list_ph, 3)> cl(comma_expand(exp_actual_list_ph, 3));
			auto_ptr<function_mem_wrapper_3<CT, RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)> > wp =
				new function_mem_wrapper_3<CT, RT, comma_expand(exp_type_list, 3), comma_expand(exp_type_list_ph, 3)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 3)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	4参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 4),
			comma_expand(exp_template_list_ph, 4)>
		function<RT(comma_expand(exp_type_list, 4))>
			bind(RT(*fun)(comma_expand(exp_type_list, 4)), comma_expand(exp_formal_list_ph, 4))
		{
			function<RT(comma_expand(exp_type_list, 4))> fo;
			CallList4<comma_expand(exp_type_list_ph, 4)> cl(comma_expand(exp_actual_list_ph, 4));
			auto_ptr<function_cstyle_wrapper_4<RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)> > wp =
				new function_cstyle_wrapper_4<RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 4)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 4),
			comma_expand(exp_template_list_ph, 4)>
		function<RT(comma_expand(exp_type_list, 4))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 4)), CT* self, comma_expand(exp_formal_list_ph, 4))
		{
			function<RT(comma_expand(exp_type_list, 4))> fo;
			CallList4<comma_expand(exp_type_list_ph, 4)> cl(comma_expand(exp_actual_list_ph, 4));
			auto_ptr<function_mem_wrapper_4<CT, RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)> > wp =
				new function_mem_wrapper_4<CT, RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 4)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 4),
			comma_expand(exp_template_list_ph, 4)>
			function<RT(comma_expand(exp_type_list, 4))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 4)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 4))
		{
			function<RT(comma_expand(exp_type_list, 4))> fo;
			CallList4<comma_expand(exp_type_list_ph, 4)> cl(comma_expand(exp_actual_list_ph, 4));
			auto_ptr<function_mem_wrapper_4<CT, RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)> > wp =
				new function_mem_wrapper_4<CT, RT, comma_expand(exp_type_list, 4), comma_expand(exp_type_list_ph, 4)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 4)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	5参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 5),
			comma_expand(exp_template_list_ph, 5)>
		function<RT(comma_expand(exp_type_list, 5))>
			bind(RT(*fun)(comma_expand(exp_type_list, 5)), comma_expand(exp_formal_list_ph, 5))
		{
			function<RT(comma_expand(exp_type_list, 5))> fo;
			CallList5<comma_expand(exp_type_list_ph, 5)> cl(comma_expand(exp_actual_list_ph, 5));
			auto_ptr<function_cstyle_wrapper_5<RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)> > wp =
				new function_cstyle_wrapper_5<RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 5)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 5),
			comma_expand(exp_template_list_ph, 5)>
			function<RT(comma_expand(exp_type_list, 5))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 5)), CT* self, comma_expand(exp_formal_list_ph, 5))
		{
			function<RT(comma_expand(exp_type_list, 5))> fo;
			CallList5<comma_expand(exp_type_list_ph, 5)> cl(comma_expand(exp_actual_list_ph, 5));
			auto_ptr<function_mem_wrapper_5<CT, RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)> > wp =
				new function_mem_wrapper_5<CT, RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 5)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 5),
			comma_expand(exp_template_list_ph, 5)>
			function<RT(comma_expand(exp_type_list, 5))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 5)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 5))
		{
			function<RT(comma_expand(exp_type_list, 5))> fo;
			CallList5<comma_expand(exp_type_list_ph, 5)> cl(comma_expand(exp_actual_list_ph, 5));
			auto_ptr<function_mem_wrapper_5<CT, RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)> > wp =
				new function_mem_wrapper_5<CT, RT, comma_expand(exp_type_list, 5), comma_expand(exp_type_list_ph, 5)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 5)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	6参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 6),
			comma_expand(exp_template_list_ph, 6)>
		function<RT(comma_expand(exp_type_list, 6))>
			bind(RT(*fun)(comma_expand(exp_type_list, 6)), comma_expand(exp_formal_list_ph, 6))
		{
			function<RT(comma_expand(exp_type_list, 6))> fo;
			CallList6<comma_expand(exp_type_list_ph, 6)> cl(comma_expand(exp_actual_list_ph, 6));
			auto_ptr<function_cstyle_wrapper_6<RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)> > wp =
				new function_cstyle_wrapper_6<RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 6)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 6),
			comma_expand(exp_template_list_ph, 6)>
		function<RT(comma_expand(exp_type_list, 6))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 6)), CT* self, comma_expand(exp_formal_list_ph, 6))
		{
			function<RT(comma_expand(exp_type_list, 6))> fo;
			CallList6<comma_expand(exp_type_list_ph, 6)> cl(comma_expand(exp_actual_list_ph, 6));
			auto_ptr<function_mem_wrapper_6<CT, RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)> > wp =
				new function_mem_wrapper_6<CT, RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 6)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 6),
			comma_expand(exp_template_list_ph, 6)>
			function<RT(comma_expand(exp_type_list, 6))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 6)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 6))
		{
			function<RT(comma_expand(exp_type_list, 6))> fo;
			CallList6<comma_expand(exp_type_list_ph, 6)> cl(comma_expand(exp_actual_list_ph, 6));
			auto_ptr<function_mem_wrapper_6<CT, RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)> > wp =
				new function_mem_wrapper_6<CT, RT, comma_expand(exp_type_list, 6), comma_expand(exp_type_list_ph, 6)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 6)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	7参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 7),
			comma_expand(exp_template_list_ph, 7)>
		function<RT(comma_expand(exp_type_list, 7))>
			bind(RT(*fun)(comma_expand(exp_type_list, 7)), comma_expand(exp_formal_list_ph, 7))
		{
			function<RT(comma_expand(exp_type_list, 7))> fo;
			CallList7<comma_expand(exp_type_list_ph, 7)> cl(comma_expand(exp_actual_list_ph, 7));
			auto_ptr<function_cstyle_wrapper_7<RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)> > wp =
				new function_cstyle_wrapper_7<RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 7)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 7),
			comma_expand(exp_template_list_ph, 7)>
		function<RT(comma_expand(exp_type_list, 7))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 7)), CT* self, comma_expand(exp_formal_list_ph, 7))
		{
			function<RT(comma_expand(exp_type_list, 7))> fo;
			CallList7<comma_expand(exp_type_list_ph, 7)> cl(comma_expand(exp_actual_list_ph, 7));
			auto_ptr<function_mem_wrapper_7<CT, RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)> > wp =
				new function_mem_wrapper_7<CT, RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 7)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 7),
			comma_expand(exp_template_list_ph, 7)>
			function<RT(comma_expand(exp_type_list, 7))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 7)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 7))
		{
			function<RT(comma_expand(exp_type_list, 7))> fo;
			CallList7<comma_expand(exp_type_list_ph, 7)> cl(comma_expand(exp_actual_list_ph, 7));
			auto_ptr<function_mem_wrapper_7<CT, RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)> > wp =
				new function_mem_wrapper_7<CT, RT, comma_expand(exp_type_list, 7), comma_expand(exp_type_list_ph, 7)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 7)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	8参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 8),
			comma_expand(exp_template_list_ph, 8)>
		function<RT(comma_expand(exp_type_list, 8))>
			bind(RT(*fun)(comma_expand(exp_type_list, 8)), comma_expand(exp_formal_list_ph, 8))
		{
			function<RT(comma_expand(exp_type_list, 8))> fo;
			CallList8<comma_expand(exp_type_list_ph, 8)> cl(comma_expand(exp_actual_list_ph, 8));
			auto_ptr<function_cstyle_wrapper_8<RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)> > wp =
				new function_cstyle_wrapper_8<RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 8)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 8),
			comma_expand(exp_template_list_ph, 8)>
		function<RT(comma_expand(exp_type_list, 8))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 8)), CT* self, comma_expand(exp_formal_list_ph, 8))
		{
			function<RT(comma_expand(exp_type_list, 8))> fo;
			CallList8<comma_expand(exp_type_list_ph, 8)> cl(comma_expand(exp_actual_list_ph, 8));
			auto_ptr<function_mem_wrapper_8<CT, RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)> > wp =
				new function_mem_wrapper_8<CT, RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 8)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 8),
			comma_expand(exp_template_list_ph, 8)>
			function<RT(comma_expand(exp_type_list, 8))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 8)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 8))
		{
			function<RT(comma_expand(exp_type_list, 8))> fo;
			CallList8<comma_expand(exp_type_list_ph, 8)> cl(comma_expand(exp_actual_list_ph, 8));
			auto_ptr<function_mem_wrapper_8<CT, RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)> > wp =
				new function_mem_wrapper_8<CT, RT, comma_expand(exp_type_list, 8), comma_expand(exp_type_list_ph, 8)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 8)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	9参数版本
		//	普通C函数绑定
		template <typename RT, comma_expand(exp_template_list, 9),
			comma_expand(exp_template_list_ph, 9)>
		function<RT(comma_expand(exp_type_list, 9))>
			bind(RT(*fun)(comma_expand(exp_type_list, 9)), comma_expand(exp_formal_list_ph, 9))
		{
			function<RT(comma_expand(exp_type_list, 9))> fo;
			CallList9<comma_expand(exp_type_list_ph, 9)> cl(comma_expand(exp_actual_list_ph, 9));
			auto_ptr<function_cstyle_wrapper_9<RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)> > wp =
				new function_cstyle_wrapper_9<RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)>(cl);
			wp->fun_	= fun;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 9)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数
		template <typename CT, typename RT, comma_expand(exp_template_list, 9),
			comma_expand(exp_template_list_ph, 9)>
		function<RT(comma_expand(exp_type_list, 9))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 9)), CT* self, comma_expand(exp_formal_list_ph, 9))
		{
			function<RT(comma_expand(exp_type_list, 9))> fo;
			CallList9<comma_expand(exp_type_list_ph, 9)> cl(comma_expand(exp_actual_list_ph, 9));
			auto_ptr<function_mem_wrapper_9<CT, RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)> > wp =
				new function_mem_wrapper_9<CT, RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)>(cl);
			wp->fun_	= fun;
			wp->self_	= self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 9)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}

		//	成员函数（自增引用计数）
		template <typename CT, typename RT, comma_expand(exp_template_list, 9),
			comma_expand(exp_template_list_ph, 9)>
			function<RT(comma_expand(exp_type_list, 9))>
			bind(RT(CT::*fun)(comma_expand(exp_type_list, 9)), auto_ptr<CT> self, comma_expand(exp_formal_list_ph, 9))
		{
			function<RT(comma_expand(exp_type_list, 9))> fo;
			CallList9<comma_expand(exp_type_list_ph, 9)> cl(comma_expand(exp_actual_list_ph, 9));
			auto_ptr<function_mem_wrapper_9<CT, RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)> > wp =
				new function_mem_wrapper_9<CT, RT, comma_expand(exp_type_list, 9), comma_expand(exp_type_list_ph, 9)>(cl);
			wp->fun_	= fun;
			wp->self_	= self.raw_ptr();
			wp->auto_self_ = self;
			wp->cl_		= cl;
			fo.invoker_ = auto_ptr<function_wrapper<RT, comma_expand(exp_type_list, 9)> >::auto_ptr_dynamic_cast(wp);
			return fo;
		}
	}
}

#endif
