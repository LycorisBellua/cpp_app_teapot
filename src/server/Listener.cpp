#include "Listener.hpp"

Listener::Listener(int fd, const std::string& ip, int port)
	: fd_(fd), ip_(ip), port_(port)
{
}

bool Listener::hasThisIP(const std::string& ip) const
{
	return this->ip_ == ip;
}

bool Listener::hasThisPort(int port) const
{
	return this->port_ == port;
}
