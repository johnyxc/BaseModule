#ifndef __SOCKET_HPP_2015_06_10__
#define __SOCKET_HPP_2015_06_10__
#include <win32_plat.hpp>
#include <activeobject.hpp>
#include <function.hpp>
#include <auto_ptr.hpp>
#include <thread.hpp>
#include <thread_pool.hpp>

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
				flag = 1;
				setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
			}
		}

		//	套接字对象
		struct socket_t : bio_bas_t<socket_t>
		{
		public :
			socket_t() : sock_() {}
			~socket_t() {}

		public :
			void bind_socket(SOCKET sock)
			{
				sock_ = sock;
			}

		private :
			SOCKET sock_;
		};

		//	解析对象
		struct resolver_t : bio_bas_t<resolver_t>
		{

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
			void asyn_connect(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				if(!ip || port == 0) return;
				if(check_url(ip))
				{	//	域名
					i_resolve(ip, port, cb, timeout);
				}
				else
				{	//	十进制IP
					i_connect(ip, port, cb, timeout);
				}
			}

		private :
			bool check_url(const char* ip)
			{
				for(unsigned int i = 0; i < strlen(ip); i++)
				{
					if((ip[i] >= 'a' && ip[i] <= 'z') ||
						(ip[i] >= 'A' && ip[i] <= 'Z')) return true;
				}
				return false;
			}

			void i_resolve(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{

			}

			void i_connect(const char* ip, unsigned short port, connect_callback cb, unsigned int timeout)
			{
				SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if(sock == INVALID_SOCKET) { cb(socket_t(), INVALID_SOCKET); return; }
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
			}

			static DWORD WINAPI i_on_connect(LPVOID param)
			{
				connect_info* ci = (connect_info*)param;
				if(ci->sock == -1) return 0;

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

				return 0;
			}

		private :
			resolver_t resolve_;
		};

		//	监听对象
		struct acceptor_t : bio_bas_t<acceptor_t>
		{

		};
	}
}

#endif
