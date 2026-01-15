#include "Socket.hpp"
#include "Log.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

/* Public (Static) ---------------------------------------------------------- */

std::pair<int, sockaddr_in> Socket::createListener(const std::string& ip,
	int port)
{
	int fd_listen = -1;
	sockaddr_in addr = {};
	if (!createSocket(fd_listen)
		|| !bindSocket(ip, port, fd_listen, addr)
		|| !listenForClients(fd_listen))
	{
		close(fd_listen);
		fd_listen = -1;
	}
	return std::pair<int, sockaddr_in>(fd_listen, addr);
}

/* Private (Static) --------------------------------------------------------- */

bool Socket::createSocket(int& fd_listen)
{
	fd_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_listen < 0)
	{
		Log::error("Error: Socket: createSocket: socket");
		return false;
	}
	else if (fcntl(fd_listen, F_SETFL, O_NONBLOCK) < 0)
	{
		Log::error("Error: Socket: createSocket: fcntl");
		return false;
	}
	return true;
}

bool Socket::bindSocket(const std::string& ip, int port, int& fd_listen,
	sockaddr_in& addr)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (ip == "localhost" || ip == "127.0.0.1")
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else if (ip == "0.0.0.0")
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if (!resolveIPv4(ip, addr))
		return false;
	if (bind(fd_listen, (sockaddr *)&addr, sizeof(addr)) < 0)
	{
		Log::error("Error: Socket: bindSocket");
		return false;
	}
	return true;
}

bool Socket::resolveIPv4(const std::string& ip, sockaddr_in& out)
{
	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICHOST;
	addrinfo* res = 0;
	if (getaddrinfo(ip.c_str(), 0, &hints, &res))
	{
		Log::error("Error: Socket: IP \"" + ip + "\" is invalid");
		return false;
	}
    out = *reinterpret_cast<sockaddr_in*>(res->ai_addr);
	freeaddrinfo(res);
	return true;
}

bool Socket::listenForClients(int fd_listen)
{
	const int queue_length = 10;
	if (listen(fd_listen, queue_length) < 0)
	{
		Log::error("Error: Socket: listenForClients");
		return false;
	}
	return true;
}
