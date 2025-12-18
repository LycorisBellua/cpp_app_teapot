#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

Server::Server(int tmp_config)
	: addr_(), max_events_(64), fd_listen_(-1), fd_epoll_(-1)
{
	/*
		TODO
		- Pass the config struct as argument.
	*/
	(void)tmp_config;

	if (!create_socket() || !bind_socket_to_port(8080)
		|| !listen_for_clients(10) || !init_event_loop() || !run_event_loop())
		return;
}

Server::~Server()
{
	close(fd_epoll_);
	close(fd_listen_);
}

bool Server::create_socket()
{
	fd_listen_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_listen_ < 0)
	{
		std::cerr << "Error: Server: create_socket" << std::endl;
		return false;
	}
	return true;
}

bool Server::bind_socket_to_port(int port)
{
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port);
	if (bind(fd_listen_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
	{
		std::cerr << "Error: Server: bind_socket_to_port" << std::endl;
		return false;
	}
	return true;
}

bool Server::listen_for_clients(int queue_length) const
{
	if (listen(fd_listen_, queue_length) < 0)
	{
		std::cerr << "Error: Server: listen_for_clients" << std::endl;
		return false;
	}
	return true;
}

bool Server::init_event_loop()
{
	fd_epoll_ = epoll_create(1);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd_listen_;
	if (epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_listen_, &ev))
	{
		std::cerr << "Error: Server: init_event_loop" << std::endl;
		return false;
	}
	return true;
}

bool Server::run_event_loop()
{
	struct epoll_event events[max_events_];
	int timeout_ms = 5000;
	while (1)
	{
		int n = epoll_wait(fd_epoll_, events, max_events_, timeout_ms);
		if (n < 0)
		{
			std::cerr << "Error: Server: run_event_loop: epoll_wait" << std::endl;
			return false;
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
			if (fd == fd_listen_)
			{
				if (!accept_new_connection())
					return false;
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
			uint32_t e = events[i].events;
			bool can_read = e & EPOLLIN;
			bool can_write = e & EPOLLOUT;
			if (can_read)
			{
				/*
					TODO
					- Currently, I fetch the request in its entirety before 
					parsing it. I need to parse each line as I go, and also to 
					check whether the body is too long (the config file has a 
					property about that).
				*/
				Client c = clients_[fd];
				char buffer[1024];
				ssize_t nread;
				while ((nread = read(fd, buffer, sizeof(buffer))) > 0)
				{
					c.req_buffer.append(buffer, nread);
					if (c.req_buffer.find("\r\n\r\n") != std::string::npos)
						break;
				}
				if (nread < 0 || c.req_buffer.empty())
				{
					close_connection(fd);
					continue;
				}
				c.last_activity = std::time(0);

				if (can_write)
				{
					std::string response = 
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: 12\r\n"
						"\r\n"
						"Hello world!";
					write(fd, response.c_str(), response.length());
					c.last_activity = std::time(0);
					/**/close_connection(fd);
				}
			}
		}
	}
	return true;
}

bool Server::accept_new_connection()
{
	int addrlen = sizeof(addr_);
	int fd_client = accept(fd_listen_, (struct sockaddr *)&addr_,
		(socklen_t *)&addrlen);
	if (fd_client < 0)
	{
		std::cerr << "Error: Server: accept_new_connection" << std::endl;
		return false;
	}
	struct epoll_event cev;
	cev.events = EPOLLIN | EPOLLOUT;
	cev.data.fd = fd_client;
	epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_client, &cev);
	std::map<int, Client>::iterator old_elem = clients_.find(fd_client);
	if (old_elem != clients_.end())
		clients_.erase(old_elem);
	struct Client c = {};
	c.last_activity = std::time(0);
	clients_.insert(std::pair<int, Client>(fd_client, c));
	return true;
}

void Server::close_connection(int fd)
{
	close(fd);
	epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd, NULL);
	clients_.erase(fd);
}
