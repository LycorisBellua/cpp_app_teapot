#pragma once

#include <netinet/in.h>
#include <string>

class Socket
{
	public:
		static int createListener(const std::string& ip, int port);
		static bool acceptConnection(int fd_listen, int& fd_client,
			sockaddr_in& addr);
		static bool makeFdNonBlocking(int fd);
		static std::string getStringIP(const sockaddr_in& addr);

	private:
		Socket();

		static bool createSocket(int& fd_listen);
		static bool bindSocket(const std::string& ip, int port, int& fd_listen,
			sockaddr_in& addr);
		static bool resolveIPv4(const std::string& ip, sockaddr_in& out);
		static bool listenForClients(int fd_listen);
};
