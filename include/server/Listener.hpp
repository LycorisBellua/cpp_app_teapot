#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <string>
#include <vector>

class Listener
{
	/*
		TODO
		- The listener has an IP and a port.
		- The listener can have more than one domain name.
		- Each domain name has a list of cookies.
		- If there's no domain, then make the domain be "", because we need a 
		list of cookies linked to domain names.

		- The Host header always gives a port, either explicitly, or implied to 
		be 80 or 443.
		- The Host header gives either the IP or the domain name.
		- Use this data to search for the listener:
			* If the Host gave an IP address, get the first domain of this IP 
			and port.
			* If the Host gave a domain, get the domain if the port matches.
	*/

	public:
		Listener(int fd, const std::string& ip, int port);

		bool hasThisIPAndPort(const std::string& ip, int port) const;
		bool addDomainName(const std::string& domain);

	private:
		Listener();

		const int fd_;
		const std::string ip_;
		const int port_;
		std::vector<std::string> domains_;
		std::vector< std::pair<std::string, std::string> > cookies_;
};

#endif
