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
		static Server* getInstance();
		static Server* getInstance(const std::string& config_path);

		~Server();

		bool run();
		bool addFdToEventHandler(int fd, bool input, bool output);
		void removeFdFromEventHandler(int fd);
		void addCgiProcess(pid_t pid, int fd_client);

	private:
		static Server* singleton_;
		static bool is_running_;
		const Router router_;
		int fd_epoll_;
		std::map<int, Listener> listeners_;
		std::map<int, Client> clients_;
		std::map<pid_t, int> cgi_processes_;
		std::map<std::string, CookieJar> jars_;
	
		Server();
		Server(const std::string& config_path);

		static void signalHandler(int signum);
		bool addListener(const std::string& ip, int port);
		void closeListeners();
		bool runEventLoop();
		CookieJar* findCookieJar(const std::string& ip);
		bool addConnection(int fd_listen);
		void closeConnection(const std::map<int, Client>::iterator& it);
		void closeIdleConnections(int idle_timeout_sec);
		void handleCgiIO(int fd);
		void handleCgiCompletion();
		void sendResponse(const std::map<int, Client>::iterator& it);
};

#endif
