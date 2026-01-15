#include "Server.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>

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
		int fd_listen = -1;
		sockaddr_in addr = {};
		if (!createSocket(fd_listen)
			|| !bindSocket(ip, port, fd_listen, addr)
			|| !listenForClients(fd_listen)
			|| !addListenerToEventHandler(fd_listen))
			close(fd_listen);
		else
			listeners_.insert(std::pair<int, sockaddr_in>(fd_listen, addr));
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

/* Private (Static) --------------------------------------------------------- */

bool Server::createSocket(int& fd_listen)
{
	fd_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_listen < 0)
	{
		Log::error("Error: Server: createSocket: socket");
		return false;
	}
	else if (fcntl(fd_listen, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd_listen);
		fd_listen = -1;
		Log::error("Error: Server: createSocket: fcntl");
		return false;
	}
	return true;
}

bool Server::bindSocket(const std::string& ip, int port, int& fd_listen,
	sockaddr_in& addr)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (ip == "localhost" || ip == "127.0.0.1")
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else if (ip == "0.0.0.0")
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if (!Server::resolveIPv4(ip, addr))
		return false;
	if (bind(fd_listen, (sockaddr *)&addr, sizeof(addr)) < 0)
	{
		Log::error("Error: Server: bindSocket");
		return false;
	}
	return true;
}

bool Server::resolveIPv4(const std::string& ip, sockaddr_in& out)
{
	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICHOST;
	addrinfo* res = 0;
	if (getaddrinfo(ip.c_str(), 0, &hints, &res))
	{
		Log::error("Error: Server: IP \"" + ip + "\" is invalid");
		return false;
	}
    out = *reinterpret_cast<sockaddr_in*>(res->ai_addr);
	freeaddrinfo(res);
	return true;
}

bool Server::listenForClients(int fd_listen)
{
	const int queue_length = 10;
	if (listen(fd_listen, queue_length) < 0)
	{
		Log::error("Error: Server: listenForClients");
		return false;
	}
	return true;
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
			bool can_read = events[i].events & EPOLLIN;
			bool can_write = events[i].events & EPOLLOUT;
			Client& c = clients_[fd];
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
