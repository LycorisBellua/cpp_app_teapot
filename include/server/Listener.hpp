#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <string>
#include <vector>

class Listener
{
	public:
		Listener(int fd, const std::string& ip, int port);

		bool hasThisIP(const std::string& ip) const;
		bool hasThisPort(int port) const;
		bool hasThisCookie(const std::pair<std::string, std::string>& pair)
			const;

	private:
		Listener();

		const int fd_;
		const std::string ip_;
		const int port_;
		std::vector< std::pair<std::string, std::string> > cookies_;
};

#endif
