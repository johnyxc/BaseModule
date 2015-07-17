#ifndef __MEMORY_HPP_2015_07_14__
#define __MEMORY_HPP_2015_07_14__
//	��װ�ڴ���䡢���󴴽��ӿ�
#include <stdlib.h>
#include <string.h>
#include <repeat.hpp>
#include <mem_pool.hpp>
#include <new>

void* mem_alloc(unsigned int size)
{
	return bas::mem_pool->alloc(size);
}

void* mem_zalloc(unsigned int size)
{
	void* p = bas::mem_pool->alloc(size);
	memset(p, 0, size);
	return p;
}

void mem_free(void* ptr)
{
	bas::mem_pool->free(ptr);
}

char* mem_strdup(const char* str)
{
	return strdup(str);
}

void* mem_copy(void* dst, void const* src, unsigned int size)
{
	return memcpy(dst, src, size);
}

void* mem_zero(void* ptr, unsigned int size)
{
	return memset(ptr, 0, size);
}

//	���󴴽�
template <typename T>
T* mem_create_object()
{
	void* p = mem_zalloc(sizeof(T));
	return new (p)T();
}

#define mem_create_decl(i) \
	template <typename T, comma_expand(exp_template_list, i)> \
	T* mem_create_object(comma_expand(exp_formal_list, i)) \
{ \
	void* p = mem_zalloc(sizeof(T)); \
	return new (p)T(comma_expand(exp_actual_list, i)); \
}

blank_expand(mem_create_decl, 9);

//	�����ͷ�
template <typename T>
void mem_delete_object(T* o)
{
	o->~T();
	mem_free(o);
}

#endif
