#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <map>
#include <netinet/in.h>

class Server
{
	public:
		Server(int tmp_config);
		~Server();

	private:
		int fd_listen_;
		int fd_epoll_;
		struct sockaddr_in addr_;
		std::map<int, Client> clients_;

		Server();
	
		bool createSocket();
		bool bindSocketToPort(int port);
		bool listenForClients(int queue_length) const;
		bool initEventLoop();
		bool runEventLoop();
		bool acceptNewConnection();
		void closeConnection(int fd);
		void closeIdleConnections(int idle_timeout_sec);
		void sendResponse(int fd, Client& c);
};

#endif
