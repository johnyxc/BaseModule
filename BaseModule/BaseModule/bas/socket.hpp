#ifndef __SOCKET_HPP_2015_06_10__
#define __SOCKET_HPP_2015_06_10__
#include <win32_plat.hpp>
#include <activeobject.hpp>
#include <function.hpp>
#include <auto_ptr.hpp>
#include <thread.hpp>
#include <thread_pool.hpp>
#include <vector>

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
				~buffer() { if(buf_ && !is_ref_) delete buf_; }

			public :
				void buffer_alloc_buf(int len)
				{
					if(buf_ && !is_ref_) delete buf_;
					buf_ = new char[len];
					is_ref_ = false;
				}

				void buffer_set_buf(char* bf)
				{
					if(buf_ && !is_ref_) delete buf_;
					buf_ = bf;
					is_ref_ = true;
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
			socket_t() : sock_() {}
			~socket_t() {}

		public :
			void bind_socket(SOCKET sock)
			{
				sock_ = sock;
			}

			void bind_error_callback(error_callback cb)
			{
				err_cb_ = cb;
			}

			//	异步接收（指定长度）
			bool asyn_recv(char* buf, int len, recv_callback cb)
			{
				if(!sock_ || !buf) return false;
				i_recv(buf, len, cb, false);
				return true;
			}

			//	异步接收（缓冲区有数据立即返回）
			bool asyn_recv_some(char* buf, int len, recv_callback cb)
			{
				if(!sock_ || !buf) return false;
				i_recv(buf, len, cb, true);
				return true;
			}

			//	异步发送
			bool asyn_send(char* buf, int len, send_callback cb)
			{
				if(!sock_ || !buf) return false;

				buffer* bf = new buffer;
				bf->buffer_alloc_buf(len);
				memcpy((void*)bf->buffer_get_buf(), (void*)buf, len);
				bf->buffer_set_len(len);

				//	每次调用都是一次性发送行为
				default_thread_pool()->post(sock_, EV_WRITE, bind(&socket_t::i_on_send, this, _1, _2, bf, cb));

				int bt = ::send(sock_, bf->buffer_get_buf(), bf->buffer_get_len(), 0);
				if(bt <= 0) { delete bf; return false; }
				bf->buffer_set_pro_len(bf->buffer_get_pro_len() + bt);

				return true;
			}

		private :
			void i_recv(char* buf, int len, recv_callback cb, bool recv_some)
			{
				buffer* bf = new buffer;
				bf->buffer_set_buf(buf);
				bf->buffer_set_len(len);
				bf->buffer_set_recv_some(recv_some);

				//	每次调用都是一次性接收行为
				default_thread_pool()->post(sock_, EV_READ, bind(&socket_t::i_on_recv, this, _1, _2, bf, cb));
			}

			void i_on_recv(evutil_socket_t sock, short type, buffer* buf, recv_callback cb)
			{
				if(!buf) return;
				int bt = ::recv(sock,
					(buf->buffer_get_buf() + buf->buffer_get_pro_len()),
					(buf->buffer_get_len() - buf->buffer_get_pro_len()),
					0);
				if(bt <= 0)
				{	//	错误事件
					delete buf;
					i_err_occur(bt);
					return;
				}

				if(buf->buffer_get_recv_some())
				{
					cb(bt, 0);
					delete buf;
				}
				else
				{
					buf->buffer_set_pro_len(buf->buffer_get_pro_len() + bt);
					if(buf->buffer_get_pro_len() == buf->buffer_get_len())
					{	//	接收完毕
						cb(buf->buffer_get_len(), 0);
						delete buf;
					}
					else
					{	//	需要持续接收
						default_thread_pool()->post(sock, EV_READ, bind(&socket_t::i_on_recv, this, _1, _2, buf, cb));
					}
				}
			}

			void i_on_send(evutil_socket_t sock, short type, buffer* buf, send_callback cb)
			{
				if(!buf) return;
				if(buf->buffer_get_pro_len() == buf->buffer_get_len())
				{	//	所有数据发送完毕
					cb(buf->buffer_get_len(), 0);
					delete buf;
				}
				else
				{	//	继续发送
					default_thread_pool()->post(sock_, EV_WRITE, bind(&socket_t::i_on_send, this, _1, _2, buf, cb));
					int bt = ::send(sock_,
						(buf->buffer_get_buf() + buf->buffer_get_pro_len()),
						(buf->buffer_get_len() - buf->buffer_get_pro_len()),
						0);
					buf->buffer_set_pro_len(buf->buffer_get_pro_len() + bt);
				}
			}

			void i_err_occur(int err)
			{
				err_cb_(err);
			}

		private :
			SOCKET sock_;
			error_callback err_cb_;
		};

		//	解析对象
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
				resolve_info* ri = new resolve_info;
				strcpy(ri->url, url);
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
						strcpy(addr.raw_ptr(), inet_ntoa(*(in_addr*)host->h_addr_list[idx++]));
						addr.raw_ptr()[15] = '\0';
						addr_list.push_back(addr);
					}
					ri->cb(addr_list, 0);
				}
				delete ri;
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
				return resolve_.asyn_resolve(ip, bind(&connector_t::i_on_resolve, this, _1, _2, port, cb, timeout));
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

				connect_info* ci = new connect_info;
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
				if(ci->sock == -1) { delete ci; return 0; }

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

				delete ci;
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
			~acceptor_t() { if(evt_) default_thread_pool()->remove(evt_); }

		public :
			bool asyn_accept(const char* ip, unsigned short port, accept_callback cb)
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
				if(::listen(sock_, 1024) != 0) { return false; }

				evt_ = default_thread_pool()->post(sock_, EV_READ | EV_PERSIST, bind(&acceptor_t::i_on_accept, this, _1, _2, cb));
				return true;
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
		};
	}
}

#endif
