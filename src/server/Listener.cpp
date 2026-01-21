#include "Listener.hpp"
#include "Host.hpp"

/* Public (Instance) -------------------------------------------------------- */

Listener::Listener(int fd, const std::string& ip, int port)
	: fd_(fd), ip_(ip), port_(port)
{
}

bool Listener::hasThisIPAndPort(const std::string& ip, int port) const
{
	return this->ip_ == ip && this->port_ == port;
}
