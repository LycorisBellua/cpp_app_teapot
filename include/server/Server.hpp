#ifndef SERVER_HPP
#define SERVER_HPP

#include "Listener.hpp"
#include "Client.hpp"
#include "Router.hpp"
#include <map>

class Server
{
	public:
		Server(const std::string& config_path);
		~Server();

	private:
		const Router router_;
		int fd_epoll_;
		std::map<int, Listener> listeners_;
		std::map<int, Client> clients_;
	
		Server();

		std::map<int, Listener>::iterator findListener(const std::string& ip,
			int port);
		std::map<int, Listener>::iterator addListener(const std::string& ip,
			int port);
		void closeListeners();
		bool addListenerToEventHandler(int fd_listen);
		bool runEventLoop();
		bool addConnection(int fd_listen);
		void closeConnection(int fd);
		void closeIdleConnections(int idle_timeout_sec);
		void sendResponse(int fd, Client& c);
};

#endif
