#include <osfunc.hpp>
#include <activeobject.hpp>
#include <bio.hpp>
#include <function.hpp>
#include <bind.hpp>
#include <auto_ptr.hpp>
#include <stdio.h>
#include <socket.hpp>
#include <demo.hpp>
#include <stdio.h>
#include <windows.h>
namespace base = bas::detail;

bas_init();

struct A : base::bio_bas_t<A>
{
	A() { printf("A Constructor\n"); }
	~A() { printf("A Destructor\n"); }

	void f0() { printf("f0\n"); }
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
	virtual ~Base() { printf("Base Destructor\n"); }
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
	~Derive() { printf("Derive Destructor\n"); }
	virtual void print() { printf("Derive\n"); }
};

base::socket_t g_sock;
char g_recv_buf[1024] = {};

void OnRecv(int bt, int err)
{
	printf("Receive Message : %s\n", g_recv_buf);

	std::string resp = g_recv_buf;
	int st = resp.find("Set-Cookie: ");
	st += strlen("Set-Cookie: ");
	int ed = resp.find("\r\n", st);
	std::string cookie = resp.substr(st, ed - st);

	static int flag = 1;
	if(flag == 1)
	{
		//std::string req = "GET /AttendanceSys/Query/PersonalPage.aspx/ HTTP/1.1\r\nHost: attendance.sicent.com\r\nConnection: keep-alive\r\nCookie: ";
		std::string req = "GET /AttendanceSys/ HTTP/1.1\r\nHost: attendance.sicent.com\r\nConnection: keep-alive\r\nCookie: ";
		req += cookie;
		req += "\r\n\r\n\r\n";
		g_sock.asyn_send(const_cast<char*>(req.c_str()), req.length(), base::socket_t::send_callback());
		flag = 0;
	}

	memset((void*)g_recv_buf, 0, sizeof(g_recv_buf));
	g_sock.asyn_recv_some(g_recv_buf, 1023, base::bind(&OnRecv, base::_1, base::_2));
}

void OnConnect(base::socket_t sock, int err)
{
	if(err == -1)
	{
		printf("connect fail\n");
	}
	else
	{
		std::string req = "POST /AttendanceSys/main/login.aspx HTTP/1.1\r\nHost: attendance.sicent.com\r\nConnection: keep-alive\r\nContent-Length: ";
		char auth_info[] = "txtUserName=yuxingchen&txtPassword=abcd1234.";
		char str_len[10] = {};
		sprintf(str_len, "%d", strlen(auth_info));
		req += str_len;
		req += "\r\n\r\n\r\n";
		req += auth_info;

		printf("connect success\n");
		g_sock = sock;
		g_sock.asyn_send(const_cast<char*>(req.c_str()), req.length(), base::socket_t::send_callback());
		memset((void*)g_recv_buf, 0, sizeof(g_recv_buf));
		g_sock.asyn_recv_some(g_recv_buf, 1023, base::bind(&OnRecv, base::_1, base::_2));
	}
}

void OnAccept(base::socket_t sock, int err)
{
	if(err == -1)
	{
		printf("Accept fail\n");
	}
	else
	{
		printf("Accept success\n");
		g_sock = sock;
		memset((void*)g_recv_buf, 0, sizeof(g_recv_buf));
		g_sock.asyn_send("aaa", strlen("aaa"), base::socket_t::send_callback());
		g_sock.asyn_recv_some(g_recv_buf, 1023, base::bind(&OnRecv, base::_1, base::_2));
	}
}

//	TODO 注意两个全局变量的编译问题
void main()
{
	{
		base::function<void ()> fo;
		{
			A* a = new A;
			fo = base::bind(&A::f0, a);
			a->release();
			fo();
		}
	}

	{
		base::auto_ptr<Base<int> > bs;
		base::auto_ptr<Derive<int> > dr = bas::make_auto_ptr<Derive<int> >();
		bs = base::auto_ptr<Base<int> >::auto_ptr_dynamic_cast(dr);
		bs->print();
	}

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
	base::connector_t co;
	//co.asyn_connect("10.34.43.55", 8869, base::bind(OnConnect, base::_1, base::_2), 5000);
	//co.asyn_connect("attendance.sicent.com", 80, base::bind(OnConnect, base::_1, base::_2), 5000);

	{
		base::acceptor_t* acpt = new base::acceptor_t;
		acpt->asyn_accept(0, 8869, bind(&OnAccept, base::_1, base::_2));
		//acpt->stop();
		//acpt->release();
	}

	base::signal<void()> sig;
	sig.slot(fo0);
	sig.slot(fo0);
	sig.slot(fo0);
	sig.slot(fo0);
	sig.slot(fo0);
	sig();

	base::signal<void(int)> sig1;
	sig1.slot(fo1);
	sig1.slot(fo1);
	sig1.slot(fo1);
	sig1(5);

	base::signal<void(int, int)> sig2;
	sig2.slot(fo2);
	sig2.slot(fo2);
	sig2.slot(fo2);
	sig2(5, 6);

	SuspendThread(GetCurrentThread());
}
