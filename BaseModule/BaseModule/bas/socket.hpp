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
#include <algorithm>
#pragma warning(disable : 4996)

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#define SOCKET_FD SOCKET
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define SOCKET_FD int
#endif

SET_MODULE_ERR_BAS(mod_sock, 0)
BEGIN_ERROR_CODE(SOCK)
END_ERROR_CODE()

namespace bas
{
	namespace detail
	{
#define MAX_BLOCK_SIZE	(1 << 14)

		//	基础套接字对象，提供一些公用函数
		struct socket_base_t
		{
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
			socket_base_t() {}
			virtual ~socket_base_t() {}

		public :
			static void set_non_block(SOCKET_FD sock)
			{
#ifdef _WIN32
				unsigned long flag = 1;
				::ioctlsocket(sock, FIONBIO, &flag);
				::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
#else
				int flag = 1;
				::fcntl(sock, F_SETFD, O_NONBLOCK);
				::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const void*)&flag, sizeof(flag));
#endif
			}

			static void set_reuse(SOCKET_FD sock)
			{
				int flag = 1;
				setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag));
			}
		};

		//	套接字收发服务
		struct socket_service_t : bio_bas_t<socket_service_t>
		{
			typedef function<void (int, int, const char*, unsigned short)> recv_callback;

			struct sock_info
			{
				SOCKET_FD sock;
				recv_callback recv_cb;
				socket_base_t::buffer* buf;
				static bool sock_cmp (const sock_info& sil, const sock_info& sir) { return sil.sock < sir.sock; }
				bool operator == (const SOCKET_FD& s) { return sock == s; }
			};

		public :
			socket_service_t() : max_sock_()
			{
				run_ = true;
				mutex_ = get_mutex();
				thread_ = mem_create_object<thread_t>(bind(&socket_service_t::work_thread, bas::retain(this)));
				thread_->run();
			}
			~socket_service_t()
			{
				run_ = false;
				thread_->join();
				release_mutex(mutex_);
			}

		public :
			static socket_service_t* instance()
			{
				if(!self_) self_ = mem_create_object<socket_service_t>();
				return self_;
			}

		public :
			void insert_socket(SOCKET_FD sock, socket_base_t::buffer* buf, const recv_callback& cb)
			{
				sock_info si;
				si.sock		= sock;
				si.recv_cb	= cb;
				si.buf		= buf;
				if(sock > max_sock_) max_sock_ = sock;

				lock(mutex_);
				std::vector<sock_info>::iterator iter;
				if((iter = std::find(sock_list_.begin(), sock_list_.end(), sock)) != sock_list_.end())
				{
					(*iter).buf = buf;
					(*iter).recv_cb = cb;
				}
				else
				{
					sock_list_.push_back(si);
					std::sort(sock_list_.begin(), sock_list_.end(), sock_info::sock_cmp);
					max_sock_ = sock_list_[sock_list_.size() - 1].sock;
				}
				unlock(mutex_);
			}

			void remove_socket(SOCKET_FD sock)
			{	//	删除不会破坏 vector 的相对顺序
				lock(mutex_);
				std::vector<sock_info>::iterator iter;
				for(iter = sock_list_.begin(); iter != sock_list_.end(); ++iter)
				{
					if((*iter).sock == sock)
					{
						sock_list_.erase(iter);
						break;
					}
				}
				max_sock_ = sock_list_[sock_list_.size() - 1].sock;
				unlock(mutex_);
			}

			void work_thread()
			{
				fd_set rd_fd;
				timeval tv = {};
				tv.tv_sec = 0;
				tv.tv_usec = 50 * 1000;
				int ret = 0;

				while(run_)
				{
					FD_ZERO(&rd_fd);
					for(unsigned int i = 0; i < sock_list_.size(); i++)
					{
						FD_SET(sock_list_[i].sock, &rd_fd);
					}
#ifdef _WIN32
					ret = ::select(0, &rd_fd, 0, 0, &tv);
#else
					ret = ::select(max_sock_+1, &rd_fd, 0, 0, &tv);
#endif
					if(ret == 0) {
						continue;
					} else {
						for(unsigned int i = 0; i < sock_list_.size(); i++)
						{
							if(FD_ISSET(sock_list_[i].sock, &rd_fd))
							{
								if(sock_list_[i].buf)
								{
									socket_base_t::buffer* buf = sock_list_[i].buf;
									char* read_buf = buf->buffer_get_buf() + buf->buffer_get_pro_len();
									int len = buf->buffer_get_len();

									sockaddr_in addr;
									socklen_t addr_len = sizeof(addr);

									ret = ::recvfrom(sock_list_[i].sock, read_buf, len, 0, (sockaddr*)&addr, &addr_len);
									if(ret > 0)
									{
										buf->buffer_set_pro_len(buf->buffer_get_pro_len() + ret);
										if(buf->buffer_get_pro_len() == buf->buffer_get_len())
										{
											sock_list_[i].recv_cb(buf->buffer_get_len(), 0, inet_ntoa(addr.sin_addr), addr.sin_port);
											sock_list_[i].buf = 0;
										}
									}
								}
							}
						}
					}
				}
			}

		private :
			static socket_service_t* self_;
			std::vector<sock_info>	 sock_list_;
			SOCKET_FD	max_sock_;
			thread_t*	thread_;
			HMUTEX		mutex_;
			bool		run_;
		};
		socket_service_t* socket_service_t::self_ = 0;

		//	大数据流辅助处理对象
		struct overlength_stream_t : bio_bas_t<overlength_stream_t>
		{
			struct stream_header
			{
				int total_len;
				int total_block;
				int block_size;
			};

			struct frame_header
			{
				int frame_len;
				int block_id;
			};

			//	用户
			typedef function<void (int, int)>			recv_callback;		//	签名：接收长度、错误码
			typedef function<void (int, int)>			send_callback;		//	签名：发送长度、错误码
			typedef function<void (int)>				error_callback;		//	签名：错误码

			//	内部逻辑
			typedef function<void (int)>				on_recv;			//	签名：接收长度
			typedef function<void (int)>				on_send;			//	签名：发送长度
			typedef function<void (int)>				on_error;			//	签名：错误码

			//	对外事件
			typedef function<bool (char*, int, bool)>		recv_action;
			typedef function<bool (const char*, int, bool)>	send_action;

		public :
			overlength_stream_t() : recv_len_(), cur_block_()
			{
				on_err_ = bind(&overlength_stream_t::i_on_error, bas::retain(this), _1);
			}
			~overlength_stream_t() {}

		public :
			void bind_recv_callback(recv_callback cb)
			{
				recv_cb_ = cb;
			}

			void bind_send_callback(send_callback cb)
			{
				send_cb_ = cb;
			}

			void bind_error_callback(error_callback cb)
			{
				err_cb_ = cb;
			}

			void set_recv_len(int len)
			{
				recv_len_ = len;
			}

			//	大数据上层一次性接收完整数据流
			//	需自行解析用户协议
			void asyn_recv(char* buf, int len)
			{
				on_recv_ = bind(&overlength_stream_t::i_on_recv, bas::retain(this), _1, buf, len);
				do_recv_((char*)&header_, sizeof(stream_header), false);
			}

			void asyn_send(const char* buf, int len)
			{
				int blocks = 0;
				((len & 0x4000) == 0) ? (blocks = len >> 14) : (blocks = (len >> 14) + 1);

				stream_header sh = {};
				sh.total_len	= len;
				sh.total_block	= blocks;
				sh.block_size	= MAX_BLOCK_SIZE;

				char* tmp_buf = (char*)mem_alloc(len);
				mem_copy((void*)tmp_buf, (void*)buf, len);

				on_send_ = bind(&overlength_stream_t::i_on_send, bas::retain(this), _1, tmp_buf, 0, sh.total_len);
				do_send_((char*)&sh, sizeof(stream_header), false);
			}

		private :
			void i_on_send(int bt, char* buf, int block, int total_len)
			{
				if(bt == 0)
				{
					send_cb_(0, 1);
					mem_free((void*)buf);
					release();
					return;
				}

				int has_sent = (block - 1) * MAX_BLOCK_SIZE + bt;
				int remain = total_len - has_sent;
				if(remain == 0)
				{
					send_cb_(total_len, 0);
					mem_free((void*)buf);
					release();
					return;
				}

				frame_header fh = {};
				fh.block_id = block;
				fh.frame_len = (remain > MAX_BLOCK_SIZE) ? MAX_BLOCK_SIZE : remain;
				char* send_buf = (char*)mem_alloc(sizeof(frame_header) + fh.frame_len);
				mem_copy((void*)send_buf, (void*)&fh, sizeof(frame_header));
				mem_copy((void*)send_buf, (void*)(buf + has_sent), fh.frame_len);

				do_send_(send_buf, sizeof(frame_header) + fh.frame_len, false);
				mem_free((void*)send_buf);
			}

			void i_on_recv(int bt, char* recv_buf, int recv_len)
			{
				if(bt == 0) return;

				if(header_.total_len != 0)
				{	//	以完整流头部数据长度字段作为接收总长
					if(recv_len < header_.total_len)
					{
						recv_buf = (char*)realloc(recv_buf, header_.total_len);
					}
					recv_len = header_.total_len;
					bt = 0;
					mem_zero((void*)&header_, sizeof(stream_header));
				}

				if(frame_.frame_len == 0)
				{
					recv_len_ += bt;
					if(recv_len_ == recv_len)
					{
						recv_cb_(recv_len, 0);
						return;
					}

					do_recv_((char*)&frame_, sizeof(frame_header), false);
				}
				else
				{
					int len = frame_.frame_len;
					mem_zero((void*)&frame_, sizeof(frame_header));
					do_recv_(recv_buf + recv_len_, len, false);
				}
			}

			void i_on_error(int ec)
			{
				err_cb_(ec);
			}

		public :
			on_recv			on_recv_;
			on_send			on_send_;
			on_error		on_err_;
			recv_action		do_recv_;
			send_action		do_send_;

		private :
			recv_callback	recv_cb_;
			send_callback	send_cb_;
			error_callback	err_cb_;
			int				recv_len_;
			int				cur_block_;
			stream_header	header_;
			frame_header	frame_;
		};

		//	UDP套接字对象
		struct udp_socket_t : public bio_bas_t<udp_socket_t>, private socket_base_t
		{
		public :
			udp_socket_t() { sock_svc_ = socket_service_t::instance(); }
			~udp_socket_t() {}

		public :
			bool init(const char* ip, unsigned short port)
			{
#ifdef _WIN32
				sock_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if(sock_ == INVALID_SOCKET) return false;
#else
				sock_ = ::socket(AF_INET, SOCK_DGRAM, 0);
				if(sock_ == -1) return false;
#endif
				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = htons(port);
				if(ip) {
					addr.sin_addr.s_addr = inet_addr(ip);
				} else {
					addr.sin_addr.s_addr = htonl(INADDR_ANY);
				}

				socket_base_t::set_non_block(sock_);
				return (::bind(sock_, (sockaddr*)&addr, sizeof(sockaddr)) == 0);
			}

			void uninit()
			{
				sock_svc_->remove_socket(sock_);
			}

			int send(const char* ip, unsigned short port, const char* buf, int len)
			{
				if(!ip || !buf) return 0;

				struct sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(ip);
				addr.sin_port = htons(port);

				return ::sendto(sock_, buf, len, 0, (sockaddr*)&addr, sizeof(sockaddr));
			}

			bool asyn_recv(char* buf, int len, const socket_service_t::recv_callback& cb)
			{
				if(!buf) return false;

				socket_base_t::buffer* send_buf = mem_create_object<socket_base_t::buffer>();
				send_buf->buffer_set_buf(buf);
				send_buf->buffer_set_len(len);

				sock_svc_->insert_socket(sock_, send_buf, cb);
				return true;
			}

		private :
			SOCKET_FD sock_;
			socket_service_t* sock_svc_;
		};

		//	TCP套接字对象
		//	对于超过 16K 的数据底层会自动分片发送或接收
		//	分片过程对上层透明
		struct socket_t : public bio_bas_t<socket_t>, private socket_base_t
		{
			typedef function<void (int, int)>	recv_callback;	//	签名：接收长度、错误码
			typedef function<void (int, int)>	send_callback;	//	签名：发送长度、错误码
			typedef function<void (int)>		error_callback;	//	签名：错误码

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
				cur_rd_evt_ = default_thread_pool()->get_event(sock_, EV_READ, bind(&socket_t::i_on_recv, bas::retain(this), _1, _2));
			}

			void bind_send_callback(send_callback cb)
			{
				send_cb_ = cb;
				send_buf_ = mem_create_object<buffer>();
				cur_wr_evt_ = default_thread_pool()->get_event(sock_, EV_WRITE, bind(&socket_t::i_on_send, bas::retain(this), _1, _2));
			}

			void bind_error_callback(error_callback cb)
			{
				err_cb_ = cb;
			}

			void get_peer_info(char* addr, int len, unsigned short* port)
			{
				struct sockaddr saddr;
				socklen_t alen = sizeof(saddr);
				if(::getpeername(sock_, &saddr, &alen)) return;

				struct sockaddr_in* saddr_in = (sockaddr_in*)&saddr;
				const char* ip = inet_ntoa(saddr_in->sin_addr);

				if(addr)
				{
					int rlen = strlen(ip);
					if(len > rlen)
					{
						strcpy(addr, ip);
						addr[rlen] = '\0';
					}
				}

				if(port)
				{
					*port = ::ntohs(saddr_in->sin_port);
				}
			}

			//	异步接收（指定长度）
			bool asyn_recv(char* buf, int len, bool slice = false)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, false, slice);
				return true;
			}

			//	异步接收（缓冲区有数据立即返回）
			bool asyn_recv_some(char* buf, int len)
			{
				if(!sock_ || !buf) return false;
				i_asyn_recv(buf, len, true, false);
				return true;
			}

			//	异步发送
			bool asyn_send(const char* buf, int len, bool slice = false)
			{
				if(!sock_ || !buf) return false;

				if(slice)
				{
					overlength_stream_t* ost = mem_create_object<overlength_stream_t>();
					ost->bind_recv_callback(recv_cb_);
					ost->bind_send_callback(send_cb_);
					ost->bind_error_callback(err_cb_);
					ost->do_recv_ = bind(&socket_t::asyn_recv, bas::retain(this), _1, _2, _3);
					ost->do_send_ = bind(&socket_t::asyn_send, bas::retain(this), _1, _2, _3);
					ost->asyn_send(buf, len);
				}
				else
				{
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
			}

			void clear()
			{
				default_thread_pool()->post(bind(&socket_t::i_on_clear, bas::retain(this)));
			}

		private :
			void i_asyn_recv(char* buf, int len, bool some, bool slice)
			{
				if(slice)
				{
					overlength_stream_t* ost = mem_create_object<overlength_stream_t>();
					ost->bind_recv_callback(recv_cb_);
					ost->bind_send_callback(send_cb_);
					ost->bind_error_callback(err_cb_);
					ost->do_recv_ = bind(&socket_t::asyn_recv, bas::retain(this), _1, _2, _3);
					ost->do_send_ = bind(&socket_t::asyn_send, bas::retain(this), _1, _2, _3);
					ost->asyn_recv(buf, len);
				}
				else
				{
					recv_buf_->clear();
					recv_buf_->buffer_set_buf(buf);
					recv_buf_->buffer_set_len(len);
					recv_buf_->buffer_set_recv_some(some);

					//	每次调用都是一次性接收行为
					default_thread_pool()->post(cur_rd_evt_);
				}
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

				thread_t* trd = mem_create_object<thread_t>(bind(&resolver_t::i_on_resolve, bas::retain(this), ri));
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

		//	连接对象
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
				int len = strlen(ip);
				for(unsigned int i = 0; i < len; i++)
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
				socket_base_t::set_non_block(sock);
				::connect(sock, (struct sockaddr*)&addr, sizeof(addr));

				connect_info* ci = mem_create_object<connect_info>();
				ci->sock = sock;
				ci->cb	 = cb;
				ci->timeout = timeout;

				thread_t* trd = mem_create_object<thread_t>(bind(&connector_t::i_on_connect, bas::retain(this), ci));
				trd->run();
				trd->release();
				return true;
			}

			void i_on_resolve(std::vector<auto_ptr<char> > addr, int err, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				if(err) { cb(socket_t(), -1); return; }
				i_connect(addr[0].raw_ptr(), port, cb, timeout);
			}

			//	Windows & Linux 都使用 select 模型
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
						{	//	连接成功
							socket_t sock;
							sock.bind_socket(ci->sock);
							ci->cb(sock, 0);
						}
					}
#else
                    if(FD_ISSET(ci->sock, &tmp_rw_fd))
					{   //	连接成功
                        socket_t sock;
                        sock.bind_socket(ci->sock);
                        ci->cb(sock, 0);
					}
#endif
				}
				else
				{	//	连接失败
					ci->cb(socket_t(), -1);
				}
				mem_delete_object(ci);
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

				socket_base_t::set_non_block(sock_);
				socket_base_t::set_reuse(sock_);

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
