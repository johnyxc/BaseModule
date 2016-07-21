#ifndef __NETPORT_H_2015_08_28__
#define __NETPORT_H_2015_08_28__
/*
*	���紫��˿ڶ�����
*
*	ְ��
*	1�������� socket ��Դ��ֻ�������Ƿ�����������
*	2���ṩ��׼ͷ�ӿڣ��ϲ�����ʵ�����нӿں���
*	3�������ϲ㴫�������ݣ������Ѱ󶨵� socket �����ݣ��ص��ϲ�
*	4������Զ������ָ����������������ص��ϲ�
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

//	��׼ͷ
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

//	�����������
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

//	���紫��˿ڶ���
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
