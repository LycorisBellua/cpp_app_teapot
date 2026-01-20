#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <utility>
#include <netinet/in.h>

class Socket
{
	public:
		static std::pair<int, sockaddr_in> createListener(const std::string& ip,
			int port);
		static std::string getStringIP(const sockaddr_in& addr);

	private:
		Socket();

		static bool createSocket(int& fd_listen);
		static bool bindSocket(const std::string& ip, int port, int& fd_listen,
			sockaddr_in& addr);
		static bool resolveIPv4(const std::string& ip, sockaddr_in& out);
		static bool listenForClients(int fd_listen);
};

#endif
