#ifndef __MEMORY_HPP_2015_07_14__
#define __MEMORY_HPP_2015_07_14__

//	封装内存分配、对象创建接口
#include <stdlib.h>
#include <string.h>
#include <repeat.hpp>
#include <mem_pool.hpp>
//#include <new>
#include <test.h>

static void* mem_alloc(unsigned int size)
{
	//return bas::detail::mem_pool_manager_t::instance()->alloc(size);
	void* p = malloc(size);
	//insert_pointer(p);
	return p;
}

static void* mem_zalloc(unsigned int size)
{
	//void* p = bas::detail::mem_pool_manager_t::instance()->alloc(size);
	void* p = malloc(size);
	memset(p, 0, size);
	//insert_pointer(p);
	return p;
}

static void mem_free(void* ptr)
{
	//bas::detail::mem_pool_manager_t::instance()->free(ptr);
	//remove_pointer(ptr);
	free(ptr);
}

static char* mem_strdup(const char* str)
{
	return strdup(str);
}

static void* mem_copy(void* dst, void const* src, unsigned int size)
{
	return memmove(dst, src, size);
}

static void* mem_zero(void* ptr, unsigned int size)
{
	return memset(ptr, 0, size);
}

//	对象创建
template <typename T>
static T* mem_create_object()
{
	void* buf = mem_alloc(sizeof(T));
	return new (buf)T();
}

#define mem_create_decl(i) \
	template <typename T, comma_expand(exp_template_list, i)> \
	T* mem_create_object(comma_expand(exp_formal_list, i)) \
{ \
	void* buf = mem_alloc(sizeof(T)); \
	return new (buf)T(comma_expand(exp_actual_list, i)); \
}

blank_expand(mem_create_decl, 9);

//	对象释放
template <typename T>
static void mem_delete_object(T* o)
{
	o->~T();
	mem_free((void*)o);
}

#endif
