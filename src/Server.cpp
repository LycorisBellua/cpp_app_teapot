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

	/* Bind socket to a port (isn't it supposed to be 80?) */
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

	int addrlen = sizeof(addr);
	// This is the smallest legal response. You must leave a blank line between 
	// the header and the body.
	//std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n"
	//	"Content-Length: 12\n\nHello world!";
	//std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n"
	//	"\nHello world!";
	//std::string hello = "HTTP/1.1 200 OK\n\nHello world!";
	//std::string hello = "HTTP/1.1 200 OK\n\n";
	//std::string hello = "HTTP/1.1 200 OK\n";
	//std::string hello = "HTTP/1.1 200 OK";
	std::string hello = "HTTP/1.1 200";
	while(1)
	{
		/* Accept next connection */
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n"
			<< std::endl;
		int fd_client = accept(fd_server, (struct sockaddr *)&addr,
			(socklen_t *)&addrlen);
		if (fd_client < 0)
		{
			std::cerr << "Error: Server: Accept failed" << std::endl;
			return ;
		}

		/* Send and receive data */
		char buffer[30000] = {0};
		long read_amount = read(fd_client, buffer, 30000);
		(void)read_amount;
		std::cout << buffer << std::endl;
		write(fd_client, hello.c_str(), hello.length());
		std::cout << "------------------Hello message sent-------------------"
			<< std::endl;

		/* Close the connection */
		close(fd_client);
	}
	/*
		EXAMPLE OF WHAT THE CLIENT SENDS

		GET /index.html HTTP/1.1
		Host: localhost:8080
		Connection: keep-alive
		Cache-Control: max-age=0
		sec-ch-ua: "Chromium";v="142", "Google Chrome";v="142", "Not_A Brand";v="99"
		sec-ch-ua-mobile: ?0
		sec-ch-ua-platform: "Linux"
		Upgrade-Insecure-Requests: 1
		User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36
		Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng;q=0.8,application/signed-exchange;v=b3;q=0.7
		Sec-Fetch-Site: none
		Sec-Fetch-Mode: navigate
		Sec-Fetch-User: ?1
		Sec-Fetch-Dest: document
		Accept-Encoding: gzip, deflate, br, zstd
		Accept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7,ja-JP;q=0.6,ja;q=0.5

		- For the sake of simplicity, you can only consider the first line in 
		the Request Headers (the method line).
		- If the file exists and the client has permission to access it, then 
		read the file and paste it within the Response as the body.
		- Don't forget to indicate the correct Content-Type value.
		- Count the body's length and indicate it with Content-Length, or don't 
		and the client will read until EOF.
		- Whether you could fetch the file or not, give the correct Status Code.
	*/
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
