/*
 * HapTcp.cpp
 *
 *  Created on: Jan 9, 2018
 *      Author: gera
 */


#include "Hap.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Hap
{
	class TcpImpl : public Tcp
	{
	private:
		std::thread task;
		bool running = false;

		int server;
		int client[Hap::MaxHttpSessions + 1];
		Hap::sid_t sess[Hap::MaxHttpSessions + 1];

		void run()
		{
			Log("Tcp::Run - enter\n");

			struct sockaddr_in address;
			size_t addrlen = sizeof(address);

			while (running)
			{
				int nfds = 0;
				fd_set readfds;
				FD_ZERO(&readfds);

				FD_SET(server, &readfds);
				nfds = server + 1;

				for (unsigned i = 0; i < sizeofarr(client); i++)
				{
					int sd = client[i];
					if (sd > 0)
					{
						FD_SET(sd, &readfds);
						if (sd >= nfds)
							nfds = sd + 1;
					}
				}

				Dbg("Tcp::Run - select\n");
				timeval to = { 1, 0 };
				int rc = ::select(nfds, &readfds, NULL, NULL, &to);
				Dbg("Tcp::Run - select: %d\n", rc);
				if (rc < 0)
				{
					Log("select error %s\n", strerror(errno));
				}

				if (rc == 0)
				{
					// timeout, process events
					for (unsigned i = 0; i < sizeofarr(client); i++)
					{
						int sd = client[i];
						if (sd == 0)
							continue;

						Hap::sid_t sid = sess[i];
						if (sid == Hap::sid_invalid)
							continue;

						Dbg("Tcp::Run - poll sid %d\n", sid);

						_http->Poll(sid, [sd](Hap::sid_t sid, char* buf, uint16_t len) -> int
						{
							if (buf != nullptr)
								return ::send(sd, buf, len, 0);
							return 0;
						});
					}
				}

				// read event on server socket - incoming connection
				if (FD_ISSET(server, &readfds))
				{
					Dbg("Tcp::Run - accept %d\n", server);

					int clnt = ::accept(server, (struct sockaddr *)&address, &addrlen);
					if (clnt < 0)
					{
						Log("accept error\n");
					}
					else
					{
						Log("Connection on socket %d from ip %s  port %d\n", clnt,
							::inet_ntoa(address.sin_addr), ntohs(address.sin_port));

						//add new socket to array of sockets
						for (unsigned i = 0; i < sizeofarr(client); i++)
						{
							if (client[i] == 0)
							{
								client[i] = clnt;
								break;
							}
						}
					}
				}

				// read event on client socket - data or disconnect
				for (unsigned i = 0; i < sizeofarr(client); i++)
				{
					int sd = client[i];

					if (FD_ISSET(sd, &readfds))
					{
						bool close = false;
						Hap::sid_t sid = sess[i];

						Dbg("Tcp::Run - data from %d\n", sd);

						if (sid == Hap::sid_invalid)
						{
							sid = _http->Open();
							sess[i] = sid;
						}

						if (sid == Hap::sid_invalid)
						{
							Log("Cannot open HTTP session for client %d\n", i);
							close = true;
						}
						else
						{
							bool rc = _http->Process(sid,
								[sd](Hap::sid_t sid, char* buf, uint16_t size) -> int
								{
									return ::recv(sd, buf, size, 0);
								},
								[sd](Hap::sid_t sid, char* buf, uint16_t len) -> int
								{
									if (buf != nullptr)
										return ::send(sd, buf, len, 0);
									return 0;
								}
							);

							if (!rc)
							{
								Log("HTTP Disconnect\n");
								close = true;
							}
						}

						if (close)
						{
							::getpeername(sd, (struct sockaddr*)&address, &addrlen);
							Log("Disconnect socket %d to ip %s  port %d\n", sd,
								::inet_ntoa(address.sin_addr), ntohs(address.sin_port));

							::close(sd);
							client[i] = 0;
							_http->Close(sid);
							sess[i] = sid_invalid;
						}
					}
				}
			}

			for (unsigned i = 0; i < sizeofarr(client); i++)
			{
				int sd = client[i];
				if (sd != 0)
				{
					::close(sd);
					client[i] = 0;
				}
			}

			Log("Tcp::Run - exit\n");
		}

	public:
		TcpImpl()
		{
			server = 0;
			for (unsigned i = 0; i < sizeofarr(client); i++)
				client[i] = 0;
		}

		~TcpImpl()
		{
			Stop();
		}

		virtual bool Start() override
		{
			for (unsigned i = 0; i < sizeofarr(client); i++)
			{
				client[i] = 0;
				sess[i] = sid_invalid;
			}

			//create the server socket
			server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (server < 0)
			{
				Log("server socket creation failed\n");
				return false;
			}

			// allow local address reuse
			int opt = 1;
			if (::setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
			{
				Log("setsockopt(server, SO_REUSEADDR) failed: %s\n", strerror(errno));
				return false;
			}

			//bind the socket
			struct sockaddr_in address;
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = Hap::config->port;

			Dbg("Tcp::Start - bind %d to %s:%d\n", server,
					::inet_ntoa(address.sin_addr), ::ntohs(address.sin_port));

			if (::bind(server, (struct sockaddr *)&address, sizeof(address))<0)
			{
				Log("bind(server, INADDR_ANY) failed\n");
				return false;
			}

			if (::listen(server, MaxHttpSessions) < 0)
			{
				Log("listen(server) failed\n");
				return false;
			}

			running = true;
			task = std::thread(&TcpImpl::run, this);

			return running;
		}

		virtual void Stop() override
		{
			running = false;

			::close(server);

			if (task.joinable())
				task.join();
		}

	} tcp;

	Tcp* Tcp::Create(Hap::Http::Server* http)
	{
		tcp._http = http;
		return &tcp;
	}
}

