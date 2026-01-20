#include "Listener.hpp"
#include "Host.hpp"

Listener::Listener(int fd, const std::string& ip, int port)
	: fd_(fd), ip_(ip), port_(port)
{
}

bool Listener::hasThisIPAndPort(const std::string& ip, int port) const
{
	return this->ip_ == ip && this->port_ == port;
}

bool Listener::addDomainName(const std::string& domain)
{
	if (!Host::isValidDomain(domain))
		return false;
	domains_.push_back(domain);
	return true;
}
