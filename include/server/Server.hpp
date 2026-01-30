#ifndef SERVER_HPP
#define SERVER_HPP

#include "Listener.hpp"
#include "Client.hpp"
#include "Router.hpp"
#include "CookieJar.hpp"
#include <map>

class Server
{
	public:
		static Server* getInstance(const std::string& config_path);

		~Server();

		bool addFdToEventHandler(int fd, bool input, bool output);
		void removeFdFromEventHandler(int fd);
		void addCgiProcess(pid_t pid, const Client& c);

	private:
		static Server* singleton_;
		const Router router_;
		int fd_epoll_;
		std::map<int, Listener> listeners_;
		std::map<int, Client> clients_;
		std::map<pid_t, Client> cgi_processes_;
		std::map<std::string, CookieJar> jars_;
	
		Server();
		Server(const std::string& config_path);

		bool addListener(const std::string& ip, int port);
		void closeListeners();
		CookieJar* findCookieJar(const std::string& ip);
		bool runEventLoop();
		bool addConnection(int fd_listen);
		void closeConnection(int fd);
		void closeIdleConnections(int idle_timeout_sec);
		void handleCgiIO(int fd);
		void handleCgiCompletion();
		void sendResponse(int fd, Client& c);
};

#endif
