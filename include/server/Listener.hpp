#pragma once

#include <string>

class Listener
{
	public:
		Listener(int fd, const std::string& ip, int port);

		bool hasThisIP(const std::string& ip) const;
		bool hasThisPort(int port) const;

	private:
		Listener();

		const int fd_;
		const std::string ip_;
		const int port_;
};
