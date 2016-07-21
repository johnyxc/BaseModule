#include "stdafx.h"
#include "netport.h"
#include <Bas/bind.hpp>
#include <Bas/bio.hpp>
#include <Bas/socket.hpp>
#include <Bas/timer.hpp>
#include <ac_win/sdo/packetparse.h>
#include <list>

struct netport_t::netport_impl_t : active_object_t<netport_impl_t>
{
	enum { CHECK_CMD_INTERVAL = 30000, CMD_TIMEOUT = 60 };

	struct cmd_info
	{
		char*	cmd_hdr;
		char*	cmd_body;
		int		body_len;
		int		aid;
		int		msg_id;
		int		flag;
	};

public :
	netport_impl_t(standard_header* hdr) : std_hdr_(hdr), flag_(), cur_send_(), sock_()
	{
		get_strand();
		mem_zero((void*)&ci_, sizeof(cmd_info));
	}
	~netport_impl_t()
	{
		if(req_cb_.valid()) req_cb_ = request_callback();
		if(err_cb_.valid()) err_cb_ = error_callback();
	}

public :
	void clear()
	{
		i_do_clear();
	}

	void bind_socket(socket_t* sock)
	{
		if(sock_ || !sock) return;

		sock_ = sock;
		sock_->bind_send_callback(bind(&netport_impl_t::i_on_send, bas::retain(this), _1, _2));
		sock_->bind_recv_callback(bind(&netport_impl_t::i_on_recv, bas::retain(this), _1, _2));
		sock_->bind_error_callback(bind(&netport_impl_t::i_on_error, bas::retain(this), _1));

		ci_.cmd_hdr = (char*)mem_alloc(std_hdr_->get_header_size());
		flag_ = 1;
		sock_->asyn_recv(ci_.cmd_hdr, std_hdr_->get_header_size());
	}

	void add_command(net_command_t* cmd)
	{
		post_call(bind(&netport_impl_t::i_add_command, bas::retain(this), cmd));
	}

	void delete_command(net_command_t* cmd)
	{
		post_call(bind(&netport_impl_t::i_delete_command, bas::retain(this), cmd));
	}

	void set_error_callback(error_callback cb)
	{
		err_cb_ = cb;
	}

	void set_request_callback(request_callback cb)
	{
		req_cb_ = cb;
	}

	void send_message(const char* buf, int len, int aid)
	{
		if(!buf) return;

		cmd_info* ci = (cmd_info*)mem_zalloc(sizeof(cmd_info));
		if(!buf || !len) {
			ci->cmd_body = 0;
		} else {
			ci->cmd_body = (char*)mem_alloc(len);
			mem_copy((void*)ci->cmd_body, (void*)buf, len);
		}
		ci->body_len = len;
		ci->aid		 = aid;

		post_call(bind(&netport_impl_t::i_send_message, bas::retain(this), ci));
	}

private :
	void i_send_message(cmd_info* ci)
	{
		if(!sock_) return;

		if(cur_send_)
		{
			cmd_info_list_.push_back(ci);
		}
		else
		{
			char* send_buf = (char*)mem_alloc(0x100000);
			int sb_len = std_hdr_->get_parser()->FormatOneCode(send_buf, 0x100000, ci->aid,
				ci->cmd_body, ci->body_len, enmCryptAlgorithm_3DES_CBC);
			if(sb_len <= 0)
			{
				mem_free((void*)ci->cmd_body);
				mem_free((void*)ci);
				return;
			}
			cur_send_ = ci;
			sock_->asyn_send(send_buf, sb_len);
			mem_free((void*)send_buf);
		}
	}

	void i_on_recv(int bt, int err)
	{
		post_call(bind(&netport_impl_t::i_on_recv_strand, bas::retain(this), bt, err));
	}

	void i_on_recv_strand(int bt, int err)
	{
		if(err || !sock_) return;

		switch(flag_)
		{
		case 0 :
			{	//	接收到完整包
				if(std_hdr_->get_parser())
				{
					char* real_msg = 0;
					unsigned int real_msg_len = 0;

					int res = std_hdr_->get_parser()->GetOneCode(ci_.cmd_body, ci_.body_len, ci_.msg_id, ci_.flag, real_msg, real_msg_len);
					if(res > 0)
					{
						i_process_command(real_msg, real_msg_len);
						mem_free((void*)real_msg);
					}
				}

				mem_free((void*)ci_.cmd_body);
				ci_.aid			= 0;
				ci_.flag		= 0;
				ci_.msg_id		= 0;
				ci_.body_len	= 0;
				ci_.cmd_body	= 0;

				flag_ = 1;
				sock_->asyn_recv(ci_.cmd_hdr, std_hdr_->get_header_size());
			}
			break;
		case 1 :
			{	//	接收包体
				standard_header* sh = std_hdr_->clone(ci_.cmd_hdr, std_hdr_->get_header_size());
				int body_len = sh->get_body_len();
				ci_.msg_id	 = sh->get_msg_id();
				ci_.flag	 = sh->get_flag();
				sh->release();

				ci_.cmd_body = (char*)mem_alloc(body_len);
				ci_.body_len = body_len;

				flag_ = 0;
				sock_->asyn_recv(ci_.cmd_body, ci_.body_len);
			}
			break;
		default : break;
		}
	}

	void i_on_send(int bt, int err)
	{
		post_call(bind(&netport_impl_t::i_on_send_strand, bas::retain(this), bt, err));
	}

	void i_on_send_strand(int bt, int err)
	{
		if(cur_send_)
		{
			if(cur_send_->cmd_hdr)	mem_free((void*)cur_send_->cmd_hdr);
			cur_send_->cmd_hdr = 0;

			if(cur_send_->cmd_body) mem_free((void*)cur_send_->cmd_body);
			cur_send_->cmd_body = 0;

			mem_free((void*)cur_send_);
			cur_send_ = 0;
		}

		if(cmd_info_list_.size())
		{
			cmd_info* ci = cmd_info_list_.front();
			cmd_info_list_.pop_front();
			i_send_message(ci);
		}
	}

	void i_on_error(int err)
	{
		i_do_clear();
		err_cb_(err);
	}

	void i_process_command(const char* body, int len)
	{
		if(!body) return;

		{
			std::list<net_command_t*>::iterator iter;
			for(iter = cmd_list_.begin(); iter != cmd_list_.end(); ++iter)
			{
				net_command_t* cmd = *iter;
				if(cmd->is_response(ci_.msg_id))
				{
					cmd->last_time_ = time(0);
					cmd->recv_cb_(body, len);
					return;
				}
			}
		}

		{	//	执行到此，说明可能是请求指令
			std::list<net_command_t*>::iterator iter;
			for(iter = cmd_request_list_.begin(); iter != cmd_request_list_.end(); ++iter)
			{
				net_command_t* cmd = *iter;
				if(cmd->is_request(ci_.msg_id) || cmd->is_response(ci_.msg_id))
				{
					cmd->last_time_ = time(0);
					cmd->recv_cb_(body, len);
					return;
				}
			}
		}

		//	新请求指令
		net_command_t* cmd = mem_create_object<net_command_t>(ci_.msg_id);
		net_command_t* tmp = cmd;

		//	上层重新创建指令对象
		req_cb_(&tmp);
		cmd->release();
		if(!tmp)
		{	//	上层不处理此指令
			return;
		}

		cmd = tmp;
		cmd->do_request_ = bind(&netport_impl_t::send_message, bas::retain(this), _1, _2, _3);
		cmd->do_close_	 = bind(&netport_impl_t::delete_command, bas::retain(this), _1);
		cmd->retain();
		cmd_request_list_.push_back(cmd);
		cmd->last_time_ = time(0);
		cmd->recv_cb_(body, len);
	}

	void i_add_command(net_command_t* cmd)
	{
		if(!cmd) return;

		if(!sock_)
		{	//	socket 对象还未正常工作
			//	直接以空消息回调上层
			cmd->recv_cb_(0, 0);
			return;
		}

		cmd->retain();
		cmd->do_request_ = bind(&netport_impl_t::send_message, bas::retain(this), _1, _2, _3);
		cmd->do_close_	 = bind(&netport_impl_t::delete_command, bas::retain(this), _1);
		cmd->last_time_	 = time(0);
		cmd_list_.push_back(cmd);
	}

	void i_delete_command(net_command_t* cmd)
	{
		std::list<net_command_t*>::iterator iter;
		for(iter = cmd_list_.begin(); iter != cmd_list_.end(); ++iter)
		{
			if(cmd == *iter)
			{
				cmd_list_.erase(iter);
				cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
				cmd->do_request_ = net_command_t::cmd_send_request();
				cmd->do_close_	 = net_command_t::cmd_delete();
				cmd->release();
				return;
			}
		}

		for(iter = cmd_request_list_.begin(); iter != cmd_request_list_.end(); ++iter)
		{
			if(cmd == *iter)
			{
				cmd_request_list_.erase(iter);
				cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
				cmd->do_request_ = net_command_t::cmd_send_request();
				cmd->do_close_	 = net_command_t::cmd_delete();
				cmd->release();
				return;
			}
		}
	}

	void i_do_clear()
	{
		if(sock_)
		{
			sock_->clear();
			sock_->release();
			sock_ = 0;
		}

		do
		{
			if(cmd_info_list_.size() == 0) break;
			std::list<cmd_info*>::iterator iter;
			for(iter = cmd_info_list_.begin(); iter != cmd_info_list_.end(); ++iter)
			{
				cmd_info* ci = (*iter);
				if(ci)
				{
					if(ci->cmd_hdr)	 mem_free((void*)ci->cmd_hdr);
					if(ci->cmd_body) mem_free((void*)ci->cmd_body);
					mem_free((void*)ci);
				}
			}
			cmd_info_list_.clear();
		} while(0);

		do
		{
			if(cmd_list_.size() == 0) break;
			std::list<net_command_t*>::iterator iter;
			for(iter = cmd_list_.begin(); iter != cmd_list_.end(); ++iter)
			{
				net_command_t* cmd = (*iter);
				if(cmd)
				{
					cmd->recv_cb_(0, 0);
					cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
					cmd->do_request_ = net_command_t::cmd_send_request();
					cmd->do_close_	 = net_command_t::cmd_delete();
					cmd->release();
				}
			}
			cmd_list_.clear();
		} while(0);

		do
		{
			if(cmd_request_list_.size() == 0) break;
			std::list<net_command_t*>::iterator iter;
			for(iter = cmd_request_list_.begin(); iter != cmd_request_list_.end(); ++iter)
			{
				net_command_t* cmd = (*iter);
				if(cmd)
				{
					cmd->recv_cb_(0, 0);
					cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
					cmd->do_request_ = net_command_t::cmd_send_request();
					cmd->do_close_	 = net_command_t::cmd_delete();
					cmd->release();
				}
			}
			cmd_request_list_.clear();
		} while(0);

		if(cur_send_)
		{
			if(cur_send_->cmd_hdr)	mem_free((void*)cur_send_->cmd_hdr);
			if(cur_send_->cmd_body) mem_free((void*)cur_send_->cmd_body);
			mem_free((void*)cur_send_);
			cur_send_ = 0;
		}

		if(ci_.cmd_hdr) mem_free((void*)ci_.cmd_hdr);
		ci_.cmd_hdr = 0;
		if(ci_.cmd_body) mem_free((void*)ci_.cmd_body);
		ci_.cmd_body = 0;
	}

	//	此函数暂未使用
	//	每隔一段时间检查命令队列
	//	处于等待中的命令对象一定在等待对方发出的回应消息，所以模拟回应消息
	void i_on_check_cmd()
	{
		long long now_time = time(0);

		std::list<net_command_t*>::iterator iter;
		for(iter = cmd_list_.begin(); iter != cmd_list_.end(); )
		{
			net_command_t* cmd = *iter;
			if(now_time - cmd->last_time_ > CMD_TIMEOUT)
			{
				cmd->recv_cb_(0, 0);
				cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
				cmd->do_request_ = net_command_t::cmd_send_request();
				cmd->do_close_	 = net_command_t::cmd_delete();
				cmd->release();
				iter = cmd_list_.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		for(iter = cmd_request_list_.begin(); iter != cmd_request_list_.end(); )
		{
			net_command_t* cmd = *iter;
			if(now_time - cmd->last_time_ > CMD_TIMEOUT)
			{
				cmd->recv_cb_(0, 0);
				cmd->recv_cb_	 = net_command_t::cmd_recv_callback();
				cmd->do_request_ = net_command_t::cmd_send_request();
				cmd->do_close_	 = net_command_t::cmd_delete();
				cmd->release();
				iter = cmd_list_.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		check_timer_.wait(bind(&netport_impl_t::i_on_check_cmd, bas::retain(this)), CHECK_CMD_INTERVAL);
	}

private :
	int							flag_;
	cmd_info					ci_;
	socket_t*					sock_;
	error_callback				err_cb_;
	request_callback			req_cb_;
	standard_header*			std_hdr_;
	cmd_info*					cur_send_;
	std::list<cmd_info*>		cmd_info_list_;
	std::list<net_command_t*>	cmd_list_;
	std::list<net_command_t*>	cmd_request_list_;
	timer_t						check_timer_;
};
//////////////////////////////////////////////////////////////////////////

netport_t::netport_t(standard_header* hdr) : impl_()
{
	impl_ = mem_create_object<netport_impl_t>(hdr);
}

netport_t::~netport_t()
{
	mem_delete_object(impl_);
}

void netport_t::bind_socket(socket_t* sock)
{
	impl_->bind_socket(sock);
}

void netport_t::set_strand(strand_t* strand)
{
	impl_->set_strand(strand);
}

void netport_t::clear()
{
	impl_->clear();
}

void netport_t::add_command(net_command_t* cmd)
{
	impl_->add_command(cmd);
}

void netport_t::delete_command(net_command_t* cmd)
{
	impl_->delete_command(cmd);
}

void netport_t::set_error_callback(error_callback cb)
{
	impl_->set_error_callback(cb);
}

void netport_t::set_request_callback(request_callback cb)
{
	impl_->set_request_callback(cb);
}

void netport_t::send_message(const char* buf, int len, int aid)
{
	impl_->send_message(buf, len, aid);
}
