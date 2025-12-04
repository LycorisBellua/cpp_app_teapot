#ifndef SERVER_HPP
#define SERVER_HPP

class Server
{
	public:
		Server();
		Server(const Server&);

		~Server();

		Server& operator=(const Server&);
};

#endif
