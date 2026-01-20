#include "Server.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

/* Public (Instance) -------------------------------------------------------- */

Server::Server(const std::string& config_path)
	: router_(Router(Config(config_path))), fd_epoll_(epoll_create(1))
{
	const std::set<std::pair<std::string, int> >& ip_ports = router_.getPorts();
	std::set<std::pair<std::string, int> >::const_iterator it;
	std::set<std::pair<std::string, int> >::const_iterator ite = ip_ports.end();
	for (it = ip_ports.begin(); it != ite; ++it)
	{
		const std::string& ip = it->first;
		int port = it->second;
		std::pair<int, sockaddr_in> listener = Socket::createListener(ip, port);
		if (listener.first != -1)
		{
			if (!addListenerToEventHandler(listener.first))
				close(listener.first);
			else
				listeners_.insert(listener);
		}
	}
	if (!runEventLoop())
		return;
}

Server::~Server()
{
	closeIdleConnections(0);
	close(fd_epoll_);
	closeListeners();
}

/* Private (Instance) ------------------------------------------------------- */

void Server::closeListeners()
{
	std::map<int, sockaddr_in>::iterator it;
	std::map<int, sockaddr_in>::iterator ite = listeners_.end();
	for (it = listeners_.begin(); it != ite; ++it)
		close(it->first);
}

bool Server::addListenerToEventHandler(int fd_listen)
{
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd_listen;
	if (epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_listen, &ev))
	{
		Log::error("Error: Server: addListenerToEventHandler");
		return false;
	}
	return true;
}

bool Server::runEventLoop()
{
	const int max_events = 64;
	const int epoll_timeout_ms = 1000;
	const int idle_timeout_sec = 30;
	epoll_event events[max_events];
	while (1)
	{
		int n = epoll_wait(fd_epoll_, events, max_events, epoll_timeout_ms);
		if (n < 0)
		{
			Log::error("Error: Server: runEventLoop: epoll_wait");
			return false;
		}
		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			std::map<int, sockaddr_in>::iterator listener = listeners_.find(fd);
			if (listener != listeners_.end())
			{
				if (!acceptNewConnection(listener->first, listener->second))
					return false;
				continue;
			}
			std::map<int, Client>::iterator client = clients_.find(fd);
			if (client == clients_.end())
				continue;
			Client& c = client->second;
			bool can_read = events[i].events & EPOLLIN;
			bool can_write = events[i].events & EPOLLOUT;
			if ((can_read || !c.isBufferEmpty()) && !c.isFullyParsed()
				&& !c.parseRequest())
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

bool Server::acceptNewConnection(int fd_listen, const sockaddr_in& addr)
{
	int addrlen = sizeof(addr);
	int fd_client = accept(fd_listen, (sockaddr *)&addr,
		(socklen_t *)&addrlen);
	if (fd_client < 0)
	{
		Log::error("Error: Server: acceptNewConnection: accept");
		return false;
	}
	else if (fcntl(fd_client, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd_client);
		Log::error("Error: Server: acceptNewConnection: fcntl");
		return false;
	}
	epoll_event cev;
	cev.events = EPOLLIN | EPOLLOUT;
	cev.data.fd = fd_client;
	epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_client, &cev);
	std::map<int, Client>::iterator old_elem = clients_.find(fd_client);
	if (old_elem != clients_.end())
		clients_.erase(old_elem);
	std::string str_ip = Socket::getStringIP(addr);
	clients_.insert(std::pair<int, Client>(fd_client,
		Client(str_ip, fd_client)));
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

void Server::sendResponse(int fd, Client& c)
{
	std::string res = Response::compose(router_, c);
	write(fd, res.c_str(), res.length());
	if (c.shouldCloseConnection())
		closeConnection(fd);
	else
	{
		c.resetParsingData();
		c.updateLastActivity();
	}
}
