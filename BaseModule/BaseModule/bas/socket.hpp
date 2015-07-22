#ifndef __SOCKET_HPP_2015_06_10__
#define __SOCKET_HPP_2015_06_10__
#include <activeobject.hpp>
#include <function.hpp>
#include <auto_ptr.hpp>
#include <thread.hpp>
#include <thread_pool.hpp>
#include <mem_pool.hpp>
#include <WinSock2.h>
#include <vector>
#pragma warning(disable : 4996)

//	TODO 注意规范错误码
namespace bas
{
	namespace detail
	{
		static void set_non_block(SOCKET sock)
		{
			{
				int flag = 1;
				setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, (int)sizeof(flag));
			}

			{
				unsigned long flag = 1;
				ioctlsocket(sock, FIONBIO, &flag);
				setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
			}
		}

		static void set_reuse(SOCKET sock)
		{
			int flag = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, (int)sizeof(flag));
		}
		//////////////////////////////////////////////////////////////////////////

		//	套接字对象
		struct socket_t : bio_bas_t<socket_t>
		{
			typedef function<void (int, int)>	recv_callback;	//	签名：接收长度、错误码
			typedef function<void (int, int)>	send_callback;	//	签名：发送长度、错误码
			typedef function<void (int)>		error_callback;	//	签名：错误码

			struct buffer
			{
			public :
				buffer() : buf_(), len_(), pro_len_(), recv_some_(), is_ref_(true) {}
				~buffer() { clear(); }

			public :
				void buffer_alloc_buf(int len)
				{
					if(buf_ && !is_ref_) mem_free(buf_);
					buf_ = (char*)mem_zalloc(len);
					is_ref_ = false;
				}

				void buffer_set_buf(char* bf)
				{
					if(buf_ && !is_ref_) mem_free(buf_);
					buf_ = bf;
					is_ref_ = true;
				}

				void clear()
				{
					if(buf_ && !is_ref_) mem_free(buf_);
					buf_		= 0;
					len_		= 0;
					pro_len_	= 0;
					recv_some_	= 0;
					is_ref_		= true;
				}

				inline void buffer_set_len(int len)			{ len_ = len; }
				inline void buffer_set_pro_len(int len)		{ pro_len_ = len; }
				inline void buffer_set_recv_some(bool b)	{ recv_some_ = b; }
				inline char* buffer_get_buf()				{ return buf_; }
				inline int	buffer_get_len()				{ return len_; }
				inline int  buffer_get_pro_len()			{ return pro_len_; }
				inline bool buffer_get_recv_some()			{ return recv_some_; }
				inline bool buffer_get_is_ref()				{ return is_ref_; }

			private :
				char*	buf_;		//	缓冲区
				int		len_;		//	处理总长度
				int		pro_len_;	//	已处理长度
				bool	recv_some_;	//	是否部分接收
				bool	is_ref_;	//	是否持有
			};
			//////////////////////////////////////////////////////////////////////////

		public :
			socket_t() : sock_(), cur_rd_evt_(), cur_wr_evt_(), recv_buf_(), send_buf_() {}
			~socket_t() {}

		public :
			void bind_socket(SOCKET sock)
			{
				if(sock <= 0) return;
				sock_ = sock;
			}

			void bind_recv_callback(recv_callback cb)
			{
				recv_cb_ = cb;
				recv_buf_ = mem_create_object<buffer>();
				cur_rd_evt_ = default_thread_pool()->get_event(sock_, EV_READ, bind(&socket_t::i_on_recv, this, _1, _2));
			}

			void bind_send_callback(send_callback cb)
			{
				send_cb_ = cb;
				send_buf_ = mem_create_object<buffer>();
				cur_wr_evt_ = default_thread_pool()->get_event(sock_, EV_WRITE, bind(&socket_t::i_on_send, this, _1, _2));
			}

			void bind_error_callback(error_callback cb)
			{
				err_cb_ = cb;
			}

			//	异步接收（指定长度）
			bool asyn_recv(char* buf, int len)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, false);
				return true;
			}

			//	异步接收（缓冲区有数据立即返回）
			bool asyn_recv_some(char* buf, int len)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, true);
				return true;
			}

			//	异步发送
			bool asyn_send(char* buf, int len)
			{
				if(!sock_ || !buf) return false;

				send_buf_->clear();
				send_buf_->buffer_alloc_buf(len);
				memcpy((void*)send_buf_->buffer_get_buf(), (void*)buf, len);
				send_buf_->buffer_set_len(len);

				int bt = ::send(sock_, send_buf_->buffer_get_buf(), send_buf_->buffer_get_len(), 0);
				if(bt <= 0)
				{
					i_on_clear();
					return false;
				}
				send_buf_->buffer_set_pro_len(send_buf_->buffer_get_pro_len() + bt);

				//	每次调用都是一次性发送行为
				default_thread_pool()->post(cur_wr_evt_);
				return true;
			}

			void clear()
			{
				default_thread_pool()->post(bind(&socket_t::i_on_clear, bas::retain(this)));
			}

		private :
			void i_asyn_recv(char* buf, int len, bool some)
			{
				recv_buf_->clear();
				recv_buf_->buffer_set_buf(buf);
				recv_buf_->buffer_set_len(len);
				recv_buf_->buffer_set_recv_some(some);

				//	每次调用都是一次性接收行为
				default_thread_pool()->post(cur_rd_evt_);
			}

			void i_on_recv(evutil_socket_t sock, short type)
			{
				int bt = ::recv(sock,
					(recv_buf_->buffer_get_buf() + recv_buf_->buffer_get_pro_len()),
					(recv_buf_->buffer_get_len() - recv_buf_->buffer_get_pro_len()),
					0);
				if(bt <= 0)
				{	//	错误事件
					i_err_occur(0, 0);
					return;
				}

				if(recv_buf_->buffer_get_recv_some())
				{
					recv_cb_(bt, 0);
				}
				else
				{
					recv_buf_->buffer_set_pro_len(recv_buf_->buffer_get_pro_len() + bt);
					if(recv_buf_->buffer_get_pro_len() == recv_buf_->buffer_get_len())
					{	//	接收完毕
						recv_cb_(recv_buf_->buffer_get_len(), 0);
					}
					else
					{	//	需要持续接收
						default_thread_pool()->post(cur_rd_evt_);
					}
				}
			}

			void i_on_send(evutil_socket_t sock, short type)
			{
				if(send_buf_->buffer_get_pro_len() == send_buf_->buffer_get_len())
				{	//	所有数据发送完毕
					send_cb_(send_buf_->buffer_get_len(), 0);
				}
				else
				{	//	继续发送
					int bt = ::send(sock_,
						(send_buf_->buffer_get_buf() + send_buf_->buffer_get_pro_len()),
						(send_buf_->buffer_get_len() - send_buf_->buffer_get_pro_len()),
						0);
					if(bt <= 0)
					{
						i_err_occur(0, 0);
						return;
					}
					send_buf_->buffer_set_pro_len(send_buf_->buffer_get_pro_len() + bt);
					default_thread_pool()->post(cur_wr_evt_);
				}
			}

			void i_err_occur(int err, event* evt)
			{
				i_on_clear();
				err_cb_(err);
			}

			void i_on_clear()
			{
				if(cur_wr_evt_)
				{
					default_thread_pool()->remove(cur_wr_evt_);
					cur_wr_evt_ = 0;
				}

				if(cur_rd_evt_)
				{
					default_thread_pool()->remove(cur_rd_evt_);
					cur_rd_evt_ = 0;
				}

				if(recv_buf_)
				{
					mem_delete_object<buffer>(recv_buf_);
					recv_buf_ = 0;
				}

				if(send_buf_)
				{
					mem_delete_object<buffer>(send_buf_);
					send_buf_ = 0;
				}

				if(sock_)
				{
					::shutdown(sock_, SD_BOTH);
					::closesocket(sock_);
					sock_ = 0;
				}
			}

		private :
			SOCKET			sock_;
			recv_callback	recv_cb_;
			send_callback	send_cb_;
			error_callback	err_cb_;
			event*			cur_wr_evt_;
			event*			cur_rd_evt_;
			buffer*			recv_buf_;
			buffer*			send_buf_;
		};

		//	域名解析对象
		struct resolver_t : bio_bas_t<resolver_t>
		{
			typedef function<void (std::vector<auto_ptr<char> >, int)> resolve_callback;
			struct resolve_info
			{
				resolve_info() {}
				char url[256];
				resolve_callback cb;
			};

		public :
			resolver_t() {}
			~resolver_t() {}

		public :
			bool asyn_resolve(const char* url, resolve_callback cb)
			{
				resolve_info* ri = mem_create_object<resolve_info>();
				strncpy(ri->url, url, strlen(url));
				ri->cb	= cb;
				::CreateThread(0, 0, i_on_resolve, (LPVOID)ri, 0, 0);
				return true;
			}

			static DWORD WINAPI i_on_resolve(LPVOID param)
			{
				resolve_info* ri = (resolve_info*)param;
				if(ri->url[0])
				{
					hostent* host = gethostbyname(ri->url);
					if(!host) { ri->cb(std::vector<auto_ptr<char> >(), -1); return 0; }

					std::vector<auto_ptr<char> > addr_list;
					int idx = 0;
					while(host->h_addr_list[idx] != 0)
					{
						auto_ptr<char> addr = new char[16];
						char* ip_addr = inet_ntoa(*(in_addr*)host->h_addr_list[idx++]);
						if(!ip_addr) continue;
						strncpy(addr.raw_ptr(), ip_addr, strlen(ip_addr));
						addr.raw_ptr()[15] = '\0';
						addr_list.push_back(addr);
					}
					ri->cb(addr_list, 0);
				}
				mem_delete_object(ri);
				return 0;
			}
		};

		//	连接对象
		struct connector_t : active_object_t<connector_t>
		{
			typedef function<void (socket_t, int)> connect_callback;
			struct connect_info
			{
				connect_info() : sock(-1), timeout() {}
				SOCKET sock;
				connect_callback cb;
				unsigned int timeout;
			};

		public :
			connector_t() {}
			~connector_t() {}

		public :
			bool asyn_connect(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				if(!ip || port == 0) { return false; }
				if(check_url(ip)) {
					i_resolve(ip, port, cb, timeout);
				} else {
					i_connect(ip, port, cb, timeout);
				}
				return true;
			}

		private :
			bool check_url(const char* ip)
			{
				for(unsigned int i = 0; i < strlen(ip); i++)
				{
					if((ip[i] >= 'a' && ip[i] <= 'z') ||
						(ip[i] >= 'A' && ip[i] <= 'Z')) { return true; }
				}
				return false;
			}

			bool i_resolve(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				return resolve_.asyn_resolve(ip, bind(&connector_t::i_on_resolve, bas::retain(this), _1, _2, port, cb, timeout));
			}

			bool i_connect(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if(sock == INVALID_SOCKET) { return false; }
				set_non_block(sock);

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(ip);
				addr.sin_port = htons(port);

				::connect(sock, (SOCKADDR*)&addr, sizeof(addr));

				connect_info* ci = mem_create_object<connect_info>();
				ci->sock = sock;
				ci->cb	 = cb;
				ci->timeout = timeout;

				::CreateThread(0, 0, i_on_connect, (LPVOID)ci, 0, 0);
				return true;
			}

			void i_on_resolve(std::vector<auto_ptr<char> > addr, int err, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				if(err) { cb(socket_t(), -1); return; }
				i_connect(addr[0].raw_ptr(), port, cb, timeout);
			}

			static DWORD WINAPI i_on_connect(LPVOID param)
			{
				connect_info* ci = (connect_info*)param;
				if(ci->sock == -1) { mem_delete_object(ci); return 0; }

				timeval tv = {};
				tv.tv_usec = 1;
				if(ci->timeout > 1) tv.tv_usec = ci->timeout * 1000;

				fd_set rw_fd, tmp_rw_fd;
				FD_ZERO(&rw_fd);
				FD_SET(ci->sock, &rw_fd);
				tmp_rw_fd = rw_fd;

				int res = ::select(0, 0, &tmp_rw_fd, 0, &tv);
				if(res > 0)
				{
					if(FD_ISSET(rw_fd.fd_array[0], &tmp_rw_fd))
					{
						if(rw_fd.fd_array[0] == ci->sock)
						{	//	连接成功
							socket_t sock;
							sock.bind_socket(ci->sock);
							ci->cb(sock, 0);
						}
					}
				}
				else
				{	//	连接失败
					ci->cb(socket_t(), -1);
				}

				mem_delete_object(ci);
				return 0;
			}

		private :
			resolver_t resolve_;
		};

		//	监听对象
		struct acceptor_t : bio_bas_t<acceptor_t>
		{
			typedef function<void (socket_t, int)> accept_callback;

		public :
			acceptor_t() : sock_(), evt_() {}
			~acceptor_t() {}

		public :
			void set_accept_callback(accept_callback cb)
			{
				acpt_cb_ = cb;
			}

			bool asyn_accept(const char* ip, unsigned short port, int backlog = 1024)
			{
				sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if(sock_ == INVALID_SOCKET) { return false; }

				set_non_block(sock_);
				set_reuse(sock_);

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = htons(port);
				if(ip) {
					addr.sin_addr.s_addr = inet_addr(ip);
				} else {
					addr.sin_addr.s_addr = htonl(INADDR_ANY);
				}

				if(::bind(sock_, (sockaddr*)&addr, sizeof(addr)) != 0) { return false; }
				if(::listen(sock_, backlog) != 0) { return false; }

				evt_ = default_thread_pool()->get_event(sock_, EV_READ | EV_PERSIST, bind(&acceptor_t::i_on_accept, bas::retain(this), _1, _2, acpt_cb_));
				default_thread_pool()->post(evt_);
				return true;
			}

			void stop()
			{
				if(evt_) default_thread_pool()->remove(evt_);
			}

		private :
			void i_on_accept(evutil_socket_t sock, short type, accept_callback cb)
			{
				sockaddr addr;
				int len = sizeof(addr);
				SOCKET client_sock = ::accept(sock, &addr, &len);

				if(client_sock == INVALID_SOCKET) { cb(socket_t(), -1); return; }
				socket_t so;
				so.bind_socket(client_sock);
				cb(so, 0);
			}

		private :
			SOCKET sock_;
			event* evt_;
			accept_callback acpt_cb_;
		};
	}
}

#endif
