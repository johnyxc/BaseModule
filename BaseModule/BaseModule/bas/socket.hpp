#ifndef __SOCKET_HPP_2015_06_10__
#define __SOCKET_HPP_2015_06_10__
#include <activeobject.hpp>
#include <function.hpp>
#include <auto_ptr.hpp>
#include <thread.hpp>
#include <thread_pool.hpp>
#include <mem_pool.hpp>
#include <error_def.hpp>
#include <vector>
#pragma warning(disable : 4996)

#ifdef _WIN32
#include <WinSock2.h>
#define SOCKET_FD SOCKET
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SOCKET_FD int
#endif

SET_MODULE_ERR_BAS(mod_sock, 0)
BEGIN_ERROR_CODE(SOCK)
END_ERROR_CODE()

namespace bas
{
	namespace detail
	{
		static void set_non_block(SOCKET_FD sock)
		{
#ifdef _WIN32
			unsigned long flag = 1;
			ioctlsocket(sock, FIONBIO, &flag);
			setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
#else
			fcntl(sock, F_SETFD, O_NONBLOCK);
#endif
		}

		static void set_reuse(SOCKET_FD sock)
		{
			int flag = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, (int)sizeof(flag));
		}
		//////////////////////////////////////////////////////////////////////////

		//	�׽��ֶ���
		struct socket_t : bio_bas_t<socket_t>
		{
			typedef function<void (int, int)>	recv_callback;	//	ǩ�������ճ��ȡ�������
			typedef function<void (int, int)>	send_callback;	//	ǩ�������ͳ��ȡ�������
			typedef function<void (int)>		error_callback;	//	ǩ����������

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
				char*	buf_;		//	������
				int		len_;		//	�����ܳ���
				int		pro_len_;	//	�Ѵ�����
				bool	recv_some_;	//	�Ƿ񲿷ֽ���
				bool	is_ref_;	//	�Ƿ����
			};
			//////////////////////////////////////////////////////////////////////////

		public :
			socket_t() : sock_(), cur_wr_evt_(), cur_rd_evt_(), recv_buf_(), send_buf_() {}
			~socket_t() {}

		public :
			void bind_socket(SOCKET_FD sock)
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

			//	�첽���գ�ָ�����ȣ�
			bool asyn_recv(char* buf, int len)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, false);
				return true;
			}

			//	�첽���գ��������������������أ�
			bool asyn_recv_some(char* buf, int len)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, true);
				return true;
			}

			//	�첽����
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

				//	ÿ�ε��ö���һ���Է�����Ϊ
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

				//	ÿ�ε��ö���һ���Խ�����Ϊ
				default_thread_pool()->post(cur_rd_evt_);
			}

			void i_on_recv(evutil_socket_t sock, short type)
			{
				int bt = ::recv(sock,
					(recv_buf_->buffer_get_buf() + recv_buf_->buffer_get_pro_len()),
					(recv_buf_->buffer_get_len() - recv_buf_->buffer_get_pro_len()),
					0);
				if(bt <= 0)
				{	//	�����¼�
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
					{	//	�������
						recv_cb_(recv_buf_->buffer_get_len(), 0);
					}
					else
					{	//	��Ҫ��������
						default_thread_pool()->post(cur_rd_evt_);
					}
				}
			}

			void i_on_send(evutil_socket_t sock, short type)
			{
				if(send_buf_->buffer_get_pro_len() == send_buf_->buffer_get_len())
				{	//	�������ݷ������
					send_cb_(send_buf_->buffer_get_len(), 0);
				}
				else
				{	//	��������
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
#ifdef _WIN32
					::shutdown(sock_, SD_BOTH);
					::closesocket(sock_);
#else
					::shutdown(sock_, SHUT_RDWR);
					::close(sock_);
#endif
					sock_ = 0;
				}
			}

		private :
			SOCKET_FD		sock_;
			recv_callback	recv_cb_;
			send_callback	send_cb_;
			error_callback	err_cb_;
			event*			cur_wr_evt_;
			event*			cur_rd_evt_;
			buffer*			recv_buf_;
			buffer*			send_buf_;
		};

		//	������������
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

				thread_t* trd = mem_create_object<thread_t>(bind(&resolver_t::i_on_resolve, this, ri));
				trd->run();
				trd->release();

				return true;
			}

			void i_on_resolve(resolve_info* ri)
			{
				if(ri->url[0])
				{
					struct hostent* host = gethostbyname(ri->url);
					if(!host) { ri->cb(std::vector<auto_ptr<char> >(), -1); return; }

					std::vector<auto_ptr<char> > addr_list;
					int idx = 0;
					while(host->h_addr_list[idx] != 0)
					{
						auto_ptr<char> addr = (char*)mem_alloc(16);
						char* ip_addr = inet_ntoa(*(in_addr*)host->h_addr_list[idx++]);
						if(!ip_addr) continue;
						strncpy(addr.raw_ptr(), ip_addr, strlen(ip_addr));
						addr.raw_ptr()[15] = '\0';
						addr_list.push_back(addr);
					}
					ri->cb(addr_list, 0);
				}
				mem_delete_object(ri);
			}
		};

		//	���Ӷ���
		struct connector_t : active_object_t<connector_t>
		{
			typedef function<void (socket_t, int)> connect_callback;
			struct connect_info
			{
				connect_info() : sock(-1), timeout() {}
				SOCKET_FD			sock;
				connect_callback	cb;
				unsigned int		timeout;
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
#ifdef _WIN32
				SOCKET_FD sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if(sock == INVALID_SOCKET) return false;

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(ip);
				addr.sin_port = htons(port);
#else
				SOCKET_FD sock = ::socket(AF_INET, SOCK_STREAM, 0);
				if(sock == -1) return false;

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				inet_pton(AF_INET, ip, &addr.sin_addr);
				addr.sin_port = htons(port);
#endif
				set_non_block(sock);
				::connect(sock, (struct sockaddr*)&addr, sizeof(addr));

				connect_info* ci = mem_create_object<connect_info>();
				ci->sock = sock;
				ci->cb	 = cb;
				ci->timeout = timeout;

				thread_t* trd = mem_create_object<thread_t>(bind(&connector_t::i_on_connect, this, ci));
				trd->run();
				trd->release();
				return true;
			}

			void i_on_resolve(std::vector<auto_ptr<char> > addr, int err, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				if(err) { cb(socket_t(), -1); return; }
				i_connect(addr[0].raw_ptr(), port, cb, timeout);
			}

			//	Windows & Linux ��ʹ�� select ģ��
			void i_on_connect(connect_info* ci)
			{
				if(ci->sock == -1) mem_delete_object(ci);

				timeval tv = {};
				tv.tv_usec = 1;
				if(ci->timeout > 1) tv.tv_usec = ci->timeout * 1000;

				fd_set rw_fd, tmp_rw_fd;
				FD_ZERO(&rw_fd);
				FD_SET(ci->sock, &rw_fd);
				tmp_rw_fd = rw_fd;

                int res = -1;
#ifdef _WIN32
				res = ::select(0, 0, &tmp_rw_fd, 0, &tv);
#else
                res = ::select(ci->sock+1, 0, &tmp_rw_fd, 0, &tv);
#endif
				if(res > 0)
				{
#ifdef _WIN32
                    if(FD_ISSET(rw_fd.fd_array[0], &tmp_rw_fd))
					{
						if(rw_fd.fd_array[0] == ci->sock)
						{	//	���ӳɹ�
							socket_t sock;
							sock.bind_socket(ci->sock);
							ci->cb(sock, 0);
						}
					}
#else
                    if(FD_ISSET(ci->sock, &tmp_rw_fd))
					{   //	���ӳɹ�
                        socket_t sock;
                        sock.bind_socket(ci->sock);
                        ci->cb(sock, 0);
					}
#endif
				}
				else
				{	//	����ʧ��
					ci->cb(socket_t(), -1);
				}
				mem_delete_object(ci);
			}

		private :
			resolver_t resolve_;
		};

		//	��������
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
#ifdef _WIN32
				sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
				sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
#endif
				if(sock_ == -1) return false;

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = htons(port);
				if(ip) {
					addr.sin_addr.s_addr = inet_addr(ip);
				} else {
					addr.sin_addr.s_addr = htonl(INADDR_ANY);
				}

				set_non_block(sock_);
				set_reuse(sock_);

				if(::bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) != 0) return false;
				if(::listen(sock_, backlog) != 0) return false;

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
				struct sockaddr addr;
#ifdef _WIN32
                int len = sizeof(addr);
				SOCKET_FD client_sock = ::accept(sock, &addr, &len);
#else
                socklen_t len = sizeof(addr);
                SOCKET_FD client_sock = ::accept(sock, &addr, &len);
#endif

				if(client_sock == -1) { cb(socket_t(), -1); return; }
				socket_t so;
				so.bind_socket(client_sock);
				cb(so, 0);
			}

		private :
			SOCKET_FD sock_;
			event* evt_;
			accept_callback acpt_cb_;
		};
	}
}

#endif
