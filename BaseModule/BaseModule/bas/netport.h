#ifndef __NETPORT_H_2015_08_28__
#define __NETPORT_H_2015_08_28__
/*
*	网络传输端口对象定义
*
*	职责：
*	1、不关心 socket 来源，只关心其是否能正常工作
*	2、提供标准头接口，上层自行实现所有接口函数
*	3、发送上层传来的数据，接收已绑定的 socket 的数据，回调上层
*	4、如遇远程请求指令，创建基本命令对象回调上层
*/
#include <Bas/activeobject.hpp>
#include <Bas/function.hpp>

namespace ac {
	namespace sdo {
		class CPacketParse;
	}
};

namespace bas {
	namespace detail {
		struct socket_t;
	}
};

using namespace bas;
using namespace bas::detail;
using namespace ac::sdo;

//	标准头
struct standard_header
{
	virtual int get_header_size()		= 0;
	virtual int get_body_len()			= 0;
	virtual int get_msg_id()			= 0;
	virtual int	get_flag()				= 0;
	virtual CPacketParse* get_parser()	= 0;
	virtual standard_header* clone(const char*, int) = 0;
	virtual void release()				= 0;
};

//	基本命令对象
struct net_command_t : bio_bas_t<net_command_t>
{
	typedef function<void (const char*, int)>		cmd_recv_callback;
	typedef function<void (const char*, int, int)>	cmd_send_request;
	typedef function<void (net_command_t*)>			cmd_delete;

public :
	net_command_t(int aid) : aid_(aid), last_time_() {}
	virtual ~net_command_t() {}

public :
	virtual bool is_response(int aid)	{ return false; }
	virtual bool is_request(int aid)	{ return false; }

public :
	int					aid_;
	cmd_recv_callback	recv_cb_;
	cmd_send_request	do_request_;
	cmd_delete			do_close_;
	long long			last_time_;
};

//	网络传输端口对象
struct netport_t : bio_bas_t<netport_t>
{
	typedef function<void (int)>			 error_callback;
	typedef function<void (net_command_t**)> request_callback;

public :
	netport_t(standard_header* hdr);
	~netport_t();

public :
	void set_strand(strand_t* strand);
	void clear();
	void bind_socket(socket_t* sock);
	void add_command(net_command_t* cmd);
	void delete_command(net_command_t* cmd);
	void set_error_callback(error_callback cb);
	void set_request_callback(request_callback cb);
	void send_message(const char* buf, int len, int aid);

private :
	struct netport_impl_t;
	netport_impl_t* impl_;
};

#endif
