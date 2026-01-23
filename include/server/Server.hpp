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

		bool addListener(const std::string& ip, int port);
		bool addListenerToEventHandler(int fd_listen);
		const Listener* findListener(const std::string& ip, int port) const;
		void closeListeners();
		bool runEventLoop();
		bool addConnection(int fd_listen);
		void closeConnection(int fd);
		void closeIdleConnections(int idle_timeout_sec);
		void sendResponse(int fd, Client& c);
};

#endif
