#ifndef __INIT_HPP_2015_07_20__
#define __INIT_HPP_2015_07_20__

void init(int count);
void uninit();

struct win32_auto_init
{
	win32_auto_init(int count);
	~win32_auto_init();
};

#define bas_init(c) \
	static win32_auto_init wai(c); \

#endif	//	__INIT_HPP_2015_07_20__
