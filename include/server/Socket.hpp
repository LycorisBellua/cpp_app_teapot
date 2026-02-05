#pragma once

#include <netinet/in.h>
#include <string>

namespace Socket
{
	int createListener(const std::string& ip, int port);
	bool acceptConnection(int fd_listen, int& fd_client, sockaddr_in& addr);
	bool makeFdNonBlocking(int fd);
	std::string getStringIP(const sockaddr_in& addr);
}
