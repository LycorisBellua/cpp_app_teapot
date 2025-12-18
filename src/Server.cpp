#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server()
{
	/* Create socket */
	int fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_server < 0)
	{
		std::cerr << "Error: Server: Socket failed" << std::endl;
		return;
	}

	/* Bind socket to a port */
	const int PORT = 8080;
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	if (bind(fd_server, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Error: Server: Bind failed" << std::endl;
		return;
	}

	/* Listen for incoming connections */
	int max_connections_before_refuse = 10;
	if (listen(fd_server, max_connections_before_refuse) < 0)
	{
		std::cerr << "Error: Server: Listen failed" << std::endl;
		return ;
	}
	//--------------------------------------------------------------------------

	fd_set master_fds;
	FD_ZERO(&master_fds);
	FD_SET(fd_server, &master_fds);
	int max_fd = fd_server;

	while (1)
	{
		struct timeval tv = { 5, 0 };
		fd_set read_fds = master_fds;
		fd_set write_fds = master_fds;
		FD_CLR(fd_server, &write_fds);
		int res_select = select(max_fd + 1, &read_fds, &write_fds, NULL, &tv);
		if (res_select < 0)
		{
			std::cerr << "Error: Server: Select failed" << std::endl;
			return ;
		}
		/*
			TODO?
			If `select` returns 0, it means that nothing happened and the 
			timer had time to run out. There's no point in running the loop 
			then, unless it's to close idle connections.
		*/
		for (int fd = 0; fd <= max_fd; ++fd)
		{
			bool can_read = FD_ISSET(fd, &read_fds);
			bool can_write = FD_ISSET(fd, &write_fds);
			if (fd == fd_server)
			{
				if (can_read)
				{
					/* Accept next connection */
					int addrlen = sizeof(addr);
					int fd_client = accept(fd_server, (struct sockaddr *)&addr,
							(socklen_t *)&addrlen);
					if (fd_client < 0)
					{
						std::cerr << "Error: Server: Accept failed" << std::endl;
						return ;
					}
					FD_SET(fd_client, &master_fds);
					max_fd = std::max(max_fd, fd_client);
				}
				continue;
			}
			/*
				TODO
				- I check for writing within the reading condition, because 
				otherwise reading wouldn't happen. In the real version, I will 
				have to check for reading and writing separately, and set a 
				condition to let me know that I have received the request, 
				because otherwise there's nothing to write of course.
				- If the request takes too long to happen, once the timeout is 
				reached remove the idle client connection.
			*/
			if (can_read)
			{
				/* Receive data */
				char buffer[30000] = {0};
				long read_amount = read(fd, buffer, 30000);
				if (read_amount <= 0)
				{
					close(fd);
					FD_CLR(fd, &master_fds);
					continue;
				}
				std::cout << buffer << std::endl;

				if (can_write)
				{
					/* Send data */
					std::string response = 
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: 12\r\n"
						"\r\n"
						"Hello world!";
					write(fd, response.c_str(), response.length());
					std::cout << "--- Response sent ---" << std::endl;

					/* Close the connection */
					close(fd);
					FD_CLR(fd, &master_fds);
				}
			}
		}
	}
}

Server::Server(const Server& other)
{
	(void)other;
}

Server::~Server()
{
}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
	}
	return *this;
}
