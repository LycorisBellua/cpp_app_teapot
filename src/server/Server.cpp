#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>

Server::Server(int tmp_config) : fd_listen_(-1), fd_epoll_(-1), addr_()
{
	/*
		TODO
		- Pass the config struct as argument to the constructor.
	*/
	int port = 8080;
	int queue_length = 10;
	(void)tmp_config;

	if (!createSocket()
		|| !bindSocketToPort(port)
		|| !listenForClients(queue_length)
		|| !initEventLoop()
		|| !runEventLoop())
		return;
}

Server::~Server()
{
	closeIdleConnections(0);
	close(fd_epoll_);
	close(fd_listen_);
}

/* Private (Instance) ------------------------------------------------------- */

bool Server::createSocket()
{
	fd_listen_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_listen_ < 0)
	{
		std::cerr << "Error: Server: createSocket: socket" << std::endl;
		return false;
	}
	else if (fcntl(fd_listen_, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd_listen_);
		std::cerr << "Error: Server: createSocket: fcntl" << std::endl;
		return false;
	}
	return true;
}

bool Server::bindSocketToPort(int port)
{
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port);
	if (bind(fd_listen_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
	{
		std::cerr << "Error: Server: bindSocketToPort" << std::endl;
		return false;
	}
	return true;
}

bool Server::listenForClients(int queue_length) const
{
	if (listen(fd_listen_, queue_length) < 0)
	{
		std::cerr << "Error: Server: listenForClients" << std::endl;
		return false;
	}
	return true;
}

bool Server::initEventLoop()
{
	fd_epoll_ = epoll_create(1);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd_listen_;
	if (epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_listen_, &ev))
	{
		std::cerr << "Error: Server: initEventLoop" << std::endl;
		return false;
	}
	return true;
}

bool Server::runEventLoop()
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
			std::cerr << "Error: Server: runEventLoop: epoll_wait"
				<< std::endl;
			return false;
		}
		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			if (fd == fd_listen_)
			{
				if (!acceptNewConnection())
					return false;
				continue;
			}
			bool can_read = events[i].events & EPOLLIN;
			bool can_write = events[i].events & EPOLLOUT;
			Client c = clients_[fd];
			if (can_read && !c.isFullyParsed() && !c.parseRequest())
			{
				closeConnection(fd);
				continue;
			}
			if (can_write && c.isFullyParsed())
				sendResponse(fd, c);
		}
		closeIdleConnections(idle_timeout_sec);
	}
	return true;
}

bool Server::acceptNewConnection()
{
	int addrlen = sizeof(addr_);
	int fd_client = accept(fd_listen_, (struct sockaddr *)&addr_,
		(socklen_t *)&addrlen);
	if (fd_client < 0)
	{
		std::cerr << "Error: Server: acceptNewConnection" << std::endl;
		return false;
	}
	else if (fcntl(fd_client, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd_client);
		std::cerr << "Error: Server: acceptNewConnection: fcntl" << std::endl;
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

void Server::closeConnection(int fd)
{
	close(fd);
	epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd, NULL);
	clients_.erase(fd);
}

void Server::closeIdleConnections(int idle_timeout_sec)
{
	std::time_t now = std::time(0);
	std::map<int, Client>::iterator it = clients_.begin();
	while (it != clients_.end())
	{
		if (now - it->second.getLastActivity() < idle_timeout_sec)
			++it;
		else
		{
			std::map<int, Client>::iterator to_erase = it;
			++it;
			closeConnection(to_erase->first);
		}
	}
}

void Server::sendResponse(int fd, Client& c) const
{
	std::string response = composeResponse(c);
	write(fd, response.c_str(), response.length());
	c.resetParsingData();
	c.updateLastActivity();
}

std::string Server::composeResponse(const Client& c) const
{
	/*
		TODO
		- The Client class containing all the parsed data is required for this 
		function to be written.
		- If the status code wasn't set, use a routing function to fetch the 
		requested resource (static page or CGI) and set the status code (e.g. 
		200, 404...).
		- If the status code represents an error, an error page needs to be 
		returned.
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
