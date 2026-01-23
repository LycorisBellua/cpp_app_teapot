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
		addListener(ip, port);
	}
	if (listeners_.empty())
		Log::error("Error: Server: constructor: listener list is empty");
	else
		runEventLoop();
}

Server::~Server()
{
	closeIdleConnections(0);
	close(fd_epoll_);
	closeListeners();
}

/* Private (Instance) ------------------------------------------------------- */

bool Server::addListener(const std::string& ip, int port)
{
	std::map<int, Listener>::iterator it;
	std::map<int, Listener>::iterator ite = listeners_.end();
	for (it = listeners_.begin(); it != ite; ++it)
	{
		if (it->second.hasThisIP(ip) && it->second.hasThisPort(port))
		{
			Log::error("Error: Server: addListener: listener already exists");
			return false;
		}
	}
	int fd_listen = Socket::createListener(ip, port);
	if (fd_listen < 0 || !addListenerToEventHandler(fd_listen))
	{
		Log::error("Error: Server: addListener: can't create listener or add "
			"it to event handler");
		close(fd_listen);
		return false;
	}
	std::pair<std::map<int, Listener>::iterator, bool> result =
		listeners_.insert(std::make_pair(fd_listen,
			Listener(fd_listen, ip, port)));
	if (!result.second)
	{
		Log::error("Error: Server: addListener: can't add listener to map");
		close(fd_listen);
		epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd_listen, NULL);
		return false;
	}
	return true;
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

const Listener* Server::findListener(const std::string& ip, int port) const
{
	bool search_ip = !ip.empty();
	bool search_port = !!port;
	std::map<int, Listener>::const_iterator it;
	std::map<int, Listener>::const_iterator ite = listeners_.end();
	std::map<int, Listener>::const_iterator it_any = ite;
	for (it = listeners_.begin(); it != ite; ++it)
	{
		bool ip_match = !search_ip || it->second.hasThisIP(ip);
		bool port_match = !search_port || it->second.hasThisPort(port);
		if (ip_match && port_match)
			return &it->second;
		else if (it_any == ite && port_match && it->second.hasThisIP("0.0.0.0"))
			it_any = it;
	}
	return it_any != ite ? &it_any->second : NULL;
}

void Server::closeListeners()
{
	std::map<int, Listener>::iterator it;
	std::map<int, Listener>::iterator ite = listeners_.end();
	for (it = listeners_.begin(); it != ite; ++it)
		close(it->first);
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
			if (listeners_.find(fd) != listeners_.end())
			{
				if (!addConnection(fd))
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

bool Server::addConnection(int fd_listen)
{
	int fd_client = -1;
	sockaddr_in addr = {};
	if (!Socket::acceptConnection(fd_listen, fd_client, addr))
		return false;
	epoll_event cev;
	cev.events = EPOLLIN | EPOLLOUT;
	cev.data.fd = fd_client;
	epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd_client, &cev);
	std::map<int, Client>::iterator old_elem = clients_.find(fd_client);
	if (old_elem != clients_.end())
		clients_.erase(old_elem);
	std::string str_ip = Socket::getStringIP(addr);
	clients_.insert(std::make_pair(fd_client, Client(str_ip, fd_client)));
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
	const Listener* listener = findListener(c.getDomain(), c.getPort());
	std::string res = Response::compose(router_, listener, c);
	write(fd, res.c_str(), res.length());
	if (c.shouldCloseConnection())
		closeConnection(fd);
	else
	{
		c.resetParsingData();
		c.updateLastActivity();
	}
}
