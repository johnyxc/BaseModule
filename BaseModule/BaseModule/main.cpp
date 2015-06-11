#include <activeobject.hpp>
#include <bio.hpp>
#include <function.hpp>
#include <bind.hpp>
#include <auto_ptr.hpp>
#include <stdio.h>
#include <socket.hpp>
#include <windows.h>
namespace base = bas::detail;

struct A
{
	A() { printf("Constructor\n"); }
	~A() { printf("Destructor\n"); }

	void f0() {}
	void f1(int i) { printf("f1 : %d\n", i); }
	void f2(int p1, int p2) { printf("f2 : %d %d\n", p1, p2); }
	void f3(int p1, char p2, int p3) { printf("f3 : %d %c %d\n", p1, p2, p3); }
	void f4(int p1, char p2, int p3, char* p4) { printf("f4 : %d %c %d %x\n", p1, p2, p3, p4); }
	void f5(int p1, char p2, float p3, int* p4, char p5) { printf("f5 : %d %c %f %d %c\n", p1, p2, p3, *p4, p5); }
	void f6(int p1, char p2, float p3, int* p4, char p5, void* p6) { printf("f6 : %d %c %f %d %c %x\n", p1, p2, p3, *p4, p5, p6); }
	void f7(int p1, double p2, float p3, int* p4, char p5, void* p6, int* p7) { printf("f7 : %d %f %f %d %c %x %d\n", p1, p2, p3, *p4, p5, p6, *p7); }
	void f8(int p1, double p2, float p3, int* p4, char p5, void* p6, int* p7, int* p8) { printf("f8 : %d %f %f %d %c %x %d %d\n", p1, p2, p3, *p4, p5, p6, *p7, *p8); }
	void f9(int p1, int p2, float p3, int* p4, char p5, void* p6, int* p7, int* p8, char* p9) { printf("f9 : %d %d %f %d %c %x %d %d %s\n", p1, p2, p3, *p4, p5, p6, *p7, *p8, p9); }

	void init()
	{
		t = new base::thread_t(base::bind(&A::work_loop, this));
		t->run();
	}

	void work_loop()
	{
		int idx = 0;
		while(1)
		{
			printf("%d\n", ++idx);
			Sleep(500);
		}
	}

	base::thread_t* t;
};

template <typename T>
struct Base : base::active_object_t<Base<T> >
{
	Base()
	{
		post(base::bind(&Base::schedule1, this, 1));
		post(base::bind(&Base::schedule2, this, 2));
	}
	virtual ~Base() {}
	virtual void print() { printf("Base\n"); }

	void schedule1(int i)
	{
		printf("In Schedule1 : %d\n", i);
		post(base::bind(&Base::schedule1, this, 1));
	}

	void schedule2(int i)
	{
		printf("In Schedule2 : %d\n", i);
		post(base::bind(&Base::schedule2, this, 2));
	}

	base::strand_t strand_;
};

template <typename T>
struct Derive : Base<T>
{
	virtual void print() { printf("Derive\n"); }
};

void OnConnect(base::socket_t sock, int err)
{
	if(err == -1)
	{
		printf("connect fail\n");
	}
	else
	{
		printf("connect success\n");
	}
}

void main()
{
	/*
	base::auto_ptr<Base<int> > bs;
	base::auto_ptr<Derive<int> > dr = bas::make_auto_ptr<Derive<int> >();
	bs = base::auto_ptr<Base<int> >::auto_ptr_dynamic_cast(dr);
	bs->print();

	A a;
 	a.init();
 	int t = 9999;
 	char* str = "abc";

	base::function<void ()> fo0 = base::bind(&A::f0, &a);
	base::function<void (int)> fo1 = base::bind(&A::f1, &a, base::_1);
	base::function<void (int, int)> fo2 = base::bind(&A::f2, &a, base::_1, base::_2);
	base::function<void (int, char, int)> fo3 = base::bind(&A::f3, &a, base::_1, base::_2, base::_3);
	base::function<void (int, char, int, char*)> fo4 = base::bind(&A::f4, &a, base::_1, base::_2, base::_3, base::_4);
	base::function<void (int, char, float, int*, char)> fo5 = base::bind(&A::f5, &a, base::_1, base::_2, base::_3, base::_4, base::_5);
	base::function<void (int, char, float, int*, char, void*)> fo6 = base::bind(&A::f6, &a, base::_1, base::_2, base::_3, base::_4, base::_5, base::_6);
	base::function<void (int, double, float, int*, char, void*, int*)> fo7 = base::bind(&A::f7, &a, base::_1, base::_2, base::_3, base::_4, base::_5, base::_6, base::_7);
	base::function<void (int, double, float, int*, char, void*, int*, int*)> fo8 = base::bind(&A::f8, &a, base::_1, base::_2, base::_3, base::_4, base::_5, base::_6, base::_7, base::_8);
	base::function<void (int, int, float, int*, char, void*, int*, int*, char*)> fo9 = base::bind(&A::f9, &a, base::_1, base::_2, base::_3, base::_4, base::_5, base::_6, base::_7, base::_8, base::_9);
	base::function<void (int, int)> fo9_2 = base::bind(&A::f9, &a, base::_2, base::_1, (float)70.0, &t, 'i', (void*)0x0, &t, &t, str);

	fo1(0);
	fo2(10, 20);
	fo3(30, 'a', 40);
	fo4(50, 'b', 60, (char*)0x100);
	fo5(70, 'c', 0.0, &t, 'd');
	fo6(80, 'e', 10.0, &t, 'f', 0x0);
	fo7(90, 20.0, 30.0, &t, 'g', 0x0, &t);
	fo8(100, 40.0, 50.0, &t, 'h', 0x0, &t, &t);
	fo9(110, 120, 70.0, &t, 'i', 0x0, &t, &t, str);
	fo9_2(120, 130);

	bas::default_thread_pool()->run();
	Base<int> b;
	//Sleep(100);

	//bas::default_thread_pool()->stop();

	int i = 0;
	*/

	bas::default_thread_pool()->run();
	base::connector_t co;
	co.asyn_connect("10.34.43.55", 8899, base::bind(OnConnect, base::_1, base::_2), 5000);

	SuspendThread(GetCurrentThread());
}
