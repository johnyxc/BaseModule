#ifndef __DEMO_HPP_2015_06_15__
#define __DEMO_HPP_2015_06_15__
#include <activeobject.hpp>
#include <function.hpp>
#include <socket.hpp>
using namespace bas::detail;

#pragma pack(push)
#pragma pack(1)
struct command_header
{
	int cmd_id;
	int cmd_len;
	int cmd_err;
};
#pragma pack(pop)

struct main_control_t : bio_bas_t<main_control_t>
{
	//	事件
	typedef signal<void (int)> watcher_care;

public :
	main_control_t() {}
	~main_control_t() {}

public :
	void init(const char* ip, unsigned short port)
	{
		ip_ = ip;
		port_ = port;
	}

	void run()
	{
		acpt_.asyn_accept(0, 8899, bind(&main_control_t::i_on_accept, bas::retain(this), _1, _2));
		connector_.asyn_connect(ip_.c_str(), port_, bind(&main_control_t::i_on_accept, bas::retain(this), _1, _2), 10000);
	}

	void uninit()
	{
		sock_c_.clear();
		sock_a_.clear();
	}

	void register_watcher(function<void (int)> watcher)
	{
		inform_.slot(watcher);
	}

private :
	void i_on_connect(socket_t sock, int err)
	{
		sock_c_ = sock;
		sock_c_.bind_error_callback(bind(&main_control_t::i_on_err, bas::retain(this), _1, 10));

	}

	void i_on_accept(socket_t sock, int err)
	{
		sock_a_ = sock;
		sock_c_.bind_error_callback(bind(&main_control_t::i_on_err, bas::retain(this), _1, 20));
		flag = 2;
		sock_c_.asyn_recv((char*)&header_, sizeof(header_), bind(&main_control_t::i_on_read, bas::retain(this), _1, _2));
	}

	void i_on_read(int bt, int err)
	{
		if(err)
		{
			sock_c_.asyn_recv((char*)&header_, sizeof(header_), bind(&main_control_t::i_on_read, bas::retain(this), _1, _2));
		}
		else
		{
			switch(flag)
			{
			case 1 :	//	接收 header
				{
					sock_c_.asyn_recv((char*)&header_, sizeof(header_), bind(&main_control_t::i_on_read, bas::retain(this), _1, _2));
					flag = 2;
				}
				break;
			case 2 :	//	接收 body
				{
					int len = header_.cmd_len;
					sock_c_.asyn_recv((char*)recv_buf_, len, bind(&main_control_t::i_on_read, bas::retain(this), _1, _2));
					flag = 1;
					i_process_command(header_, recv_buf_);
				}
				break;
			default :
				break;
			}
		}
	}

	void i_process_command(command_header& header, const char* body)
	{
		//...

		if(header_.cmd_id == 1)
		{
			inform_(header_.cmd_id);
		}

		//...
	}

	void i_on_write(int bt, int err)
	{
		//...
	}

	void i_on_err(int err, int id)
	{
		//	reconnect
	}

private :
	acceptor_t	acpt_;
	connector_t connector_;
	socket_t	sock_c_;
	socket_t	sock_a_;
	std::string ip_;
	unsigned short port_;

	command_header header_;
	char recv_buf_[1024];
	int flag;

	watcher_care inform_;
};

#endif
