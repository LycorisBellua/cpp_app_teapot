#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

Server::Server(int tmp_config) : fd_listen_(-1), fd_epoll_(-1), addr_()
{
	/*
		TODO
		- Pass the config struct as argument to the constructor.
	*/
	int port = 8080;
	int queue_length = 10;
	(void)tmp_config;

	if (!create_socket()
		|| !bind_socket_to_port(port)
		|| !listen_for_clients(queue_length)
		|| !init_event_loop()
		|| !run_event_loop())
		return;
}

Server::~Server()
{
	close_idle_connections(0);
	close(fd_epoll_);
	close(fd_listen_);
}

/* Private (Instance) ------------------------------------------------------- */

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
	/*
		TODO
		- From the terminal and not the browser, check that the server can 
		handle two requests with the same connection, instead of two separate 
		connections being necessary. Maybe leave a debug output in 
		`close_connection` to check how many connections were closed (which 
		should be 1).
	*/
	const int max_events = 64;
	const int epoll_timeout_ms = 1000; // 1s
	const int idle_timeout_sec = 10;
	struct epoll_event events[max_events];
	while (1)
	{
		int n = epoll_wait(fd_epoll_, events, max_events, epoll_timeout_ms);
		if (n < 0)
		{
			std::cerr << "Error: Server: run_event_loop: epoll_wait"
				<< std::endl;
			return false;
		}
		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			if (fd == fd_listen_)
			{
				if (!accept_new_connection())
					return false;
				continue;
			}
			bool can_read = events[i].events & EPOLLIN;
			bool can_write = events[i].events & EPOLLOUT;
			Client c = clients_[fd];
			if (can_read && !c.get_is_parsed())
			{
				if (!c.parse_request())
				{
					close_connection(fd);
					continue;
				}
			}
			if (can_write && c.get_is_parsed())
				send_response(fd, c);
		}
		close_idle_connections(idle_timeout_sec);
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
	clients_.insert(std::pair<int, Client>(fd_client, Client(fd_client)));
	return true;
}

void Server::close_connection(int fd)
{
	close(fd);
	epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd, NULL);
	clients_.erase(fd);
}

void Server::close_idle_connections(int idle_timeout_sec)
{
	std::time_t now = std::time(0);
	std::map<int, Client>::iterator it = clients_.begin();
	while (it != clients_.end())
	{
		if (now - it->second.get_last_activity() < idle_timeout_sec)
			++it;
		else
		{
			std::map<int, Client>::iterator to_erase = it;
			++it;
			close_connection(to_erase->first);
		}
	}
}

void Server::send_response(int fd, Client& c) const
{
	std::string response = compose_response(c);
	write(fd, response.c_str(), response.length());
	c.reset_req_data();
	c.update_last_activity();
}

std::string Server::compose_response(const Client& c) const
{
	/*
		TODO
		- The Client class containing all the parsed data is required for this 
		function to be written.
		- Compose the response.
	*/
	(void)c;
	std::string response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 12\r\n"
		"\r\n"
		"Hello world!";
	return response;
}
