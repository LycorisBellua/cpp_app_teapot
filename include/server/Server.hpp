#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Router.hpp"
#include <map>
#include <netinet/in.h>

class Server
{
	public:
		Server(const std::string& config_path);
		~Server();

	private:
		const Router router_;
		int fd_epoll_;
		std::map<int, sockaddr_in> listeners_;
		std::map<int, Client> clients_;
	
		Server();

		void closeListeners();
		bool addListenerToEventHandler(int fd_listen);
		bool runEventLoop();
		bool acceptNewConnection(int fd_listen, const sockaddr_in& addr);
		void closeConnection(int fd);
		void closeIdleConnections(int idle_timeout_sec);
		void sendResponse(int fd, Client& c);

		static bool createSocket(int& fd_listen);
		static bool bindSocket(const std::string& ip, int port, int& fd_listen,
			sockaddr_in& addr);
		static bool resolveIPv4(const std::string& ip, sockaddr_in& out);
		static bool listenForClients(int fd_listen);
};

#endif
