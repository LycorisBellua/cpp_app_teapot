#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

class Server
{
	public:
		Server(int tmp_config);
		~Server();
	
	private:
		struct sockaddr_in addr_;
		const int max_events_;
		int fd_listen_;
		int fd_epoll_;

		Server();
	
		bool create_socket();
		bool bind_socket_to_port(int port);
		bool listen_for_clients(int queue_length) const;
		bool init_event_loop();
		bool run_event_loop() const;
		bool accept_new_connection() const;
};

#endif
