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
	
		bool create_socket();
		bool bind_socket_to_port(int port);
		bool listen_for_clients(int queue_length) const;
		bool init_event_loop();
		bool run_event_loop();
		bool accept_new_connection();
		void close_connection(int fd);
		void close_idle_connections(int idle_timeout_sec);
		bool read_more_request_data(int fd, Client& c);
		void send_response(int fd, Client& c);
		std::string compose_response(const Client& c) const;
};

#endif
