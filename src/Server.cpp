#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

Server::Server()
{
	/* Create socket */
	int fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_server < 0)
	{
		std::cerr << "Error: Server: socket failed" << std::endl;
		return;
	}

	/* Bind socket to a port */
	const int PORT = 8080;
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	if (bind(fd_server, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Error: Server: bind failed" << std::endl;
		return;
	}

	/* Listen for incoming connections */
	int max_connections_before_refuse = 10;
	if (listen(fd_server, max_connections_before_refuse) < 0)
	{
		std::cerr << "Error: Server: listen failed" << std::endl;
		return;
	}
	//--------------------------------------------------------------------------

	int ep_fd = epoll_create(1);

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd_server;
	if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd_server, &ev))
	{
		std::cerr << "Error: Server: epoll_ctl failed to add fd_server"
			<< std::endl;
		return;
	}

	const int MAX_EVENTS = 64;
	struct epoll_event events[MAX_EVENTS];

	while (1)
	{
		int n = epoll_wait(ep_fd, events, MAX_EVENTS, 5000);
		if (n < 0)
		{
			std::cerr << "Error: Server: epoll_wait failed" << std::endl;
			return;
		}
		/*
			TODO?
			If `epoll_wait` returns 0, it means that nothing happened and the 
			timer had time to run out. There's no point in running the loop 
			then, unless it's to close idle connections.
		*/

		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			uint32_t e = events[i].events;
			bool can_read = e & EPOLLIN;
			bool can_write = e & EPOLLOUT;

			if (fd == fd_server)
			{
				/* Accept next connection */
				int addrlen = sizeof(addr);
				int fd_client = accept(fd_server, (struct sockaddr *)&addr,
					(socklen_t *)&addrlen);
				if (fd_client < 0)
				{
					std::cerr << "Error: Server: accept failed" << std::endl;
					return ;
				}

				struct epoll_event cev;
				cev.events = EPOLLIN | EPOLLOUT;
				cev.data.fd = fd_client;
				epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd_client, &cev);
				continue;
			}
			/*
				TODO
				- I check for writing within the reading condition, because 
				otherwise reading wouldn't happen. In the real version, I will 
				have to check for reading and writing separately, and set a 
				condition to let me know that I have received the request, 
				because otherwise there's nothing to write of course.
				- If the request takes too long to happen, once the timeout is 
				reached remove the idle client connection.
			*/
			if (can_read)
			{
				/* Receive data */
				char buffer[30000] = {0};
				ssize_t nread = read(fd, buffer, sizeof(buffer));
				if (nread <= 0)
				{
					close(fd);
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
					continue;
				}
				std::cout << buffer << std::endl;

				if (can_write)
				{
					/* Send data */
					std::string response = 
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: 12\r\n"
						"\r\n"
						"Hello world!";
					write(fd, response.c_str(), response.length());
					std::cout << "--- Response sent ---" << std::endl;
					close(fd);
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
				}
			}
		}
	}

	close(ep_fd);
}

Server::Server(const Server& other)
{
	(void)other;
}

Server::~Server()
{
}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
	}
	return *this;
}
