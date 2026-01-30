#include "Server.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Response.hpp"
#include "Cgi.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

Server* Server::singleton_ = NULL;

/* Public (Static) ---------------------------------------------------------- */

Server* Server::getInstance(const std::string& config_path)
{
	if (!singleton_)
		singleton_ = new Server(config_path);
	return singleton_;
}

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
		if (addListener(ip, port) && jars_.find(ip) == jars_.end())
			jars_.insert(std::make_pair(ip, CookieJar(ip)));
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

bool Server::addFdToEventHandler(int fd, bool input, bool output)
{
	if (!input && !output)
	{
		Log::error("Error: Server: addFdToEventHandler: no I/O specified");
		return false;
	}
	epoll_event ev;
	ev.events = 0;
	if (input)
		ev.events |= EPOLLIN;
	if (output)
		ev.events |= EPOLLOUT;
	ev.data.fd = fd;
	if (epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd, &ev))
	{
		Log::error("Error: Server: addFdToEventHandler: epoll_ctl");
		return false;
	}
	return true;
}

void Server::removeFdFromEventHandler(int fd)
{
	epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd, NULL);
}

void Server::addCgiProcess(pid_t pid, const Client& c)
{
	cgi_processes_.insert(std::make_pair(pid, c));
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
	if (fd_listen < 0 || !addFdToEventHandler(fd_listen, true, false))
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
		removeFdFromEventHandler(fd_listen);
		return false;
	}
	return true;
}

void Server::closeListeners()
{
	std::map<int, Listener>::iterator it;
	std::map<int, Listener>::iterator ite = listeners_.end();
	for (it = listeners_.begin(); it != ite; ++it)
		close(it->first);
}

CookieJar* Server::findCookieJar(const std::string& ip)
{
	std::map<std::string, CookieJar>::iterator ite = jars_.end();
	std::map<std::string, CookieJar>::iterator it = jars_.find(ip);
	if (it == ite)
		it = jars_.find("0.0.0.0");
	return it != ite ? &it->second : NULL;
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
			bool can_read = events[i].events & EPOLLIN;
			bool can_write = events[i].events & EPOLLOUT;
			if (listeners_.find(fd) != listeners_.end())
			{
				if (!addConnection(fd))
					return false;
				continue;
			}
			std::map<int, Client>::iterator client = clients_.find(fd);
			if (client == clients_.end())
			{
				handleCgiIO(fd);
				continue;
			}
			Client& c = client->second;
			if ((can_read || !c.isBufferEmpty()) && !c.isFullyParsed()
				&& !c.parseRequest())
			{
				closeConnection(fd);
				continue;
			}
			if (can_write && c.isFullyParsed() && !c.isCgiRunning())
				sendResponse(fd, c);
		}
		closeIdleConnections(idle_timeout_sec);
		handleCgiCompletion();
	}
	return true;
}

bool Server::addConnection(int fd_listen)
{
	int fd_client = -1;
	sockaddr_in addr = {};
	if (!Socket::acceptConnection(fd_listen, fd_client, addr))
		return false;
	addFdToEventHandler(fd_client, true,  true);
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
	removeFdFromEventHandler(fd);
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

void Server::handleCgiIO(int fd)
{
	std::map<int, Client>::iterator it;
	std::map<int, Client>::iterator ite = clients_.end();
	for (it = clients_.begin(); it != ite; ++it)
	{
		Client& c = it->second;
		if (fd == c.getCgiFdOutput())
		{
			Cgi::writeToCgi(*c.route_info);
			break;
		}
		else if (fd == c.getCgiFdInput())
		{
			if (c.getCgiFdOutput() < 0)
				Cgi::readFromCgi(*c.route_info);
			break;
		}
	}
}

void Server::handleCgiCompletion()
{
	std::map<pid_t, Client>::iterator it;
	std::map<pid_t, Client>::iterator ite = cgi_processes_.end();
	for (it = cgi_processes_.begin(); it != ite;)
	{
		Client& c = it->second;
		c.response_data = Cgi::reapCgiProcess(*c.route_info);
		if (!c.response_data)
			++it;
		else
		{
			std::map<pid_t, Client>::iterator it_next = it;
			++it_next;
			cgi_processes_.erase(it);
			it = it_next;
		}
	}
}

void Server::sendResponse(int fd, Client& c)
{
	CookieJar* jar = findCookieJar(c.getDomain());
	std::string res = Response::compose(router_, jar, c);
	if (c.isCgiRunning())
		return;
	write(fd, res.c_str(), res.length());
	if (c.shouldCloseConnection())
		closeConnection(fd);
	else
	{
		c.resetParsingData();
		c.updateLastActivity();
	}
}
