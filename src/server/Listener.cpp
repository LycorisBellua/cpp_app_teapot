#include "Listener.hpp"
#include "Host.hpp"
#include "HexColorCode.hpp"
#include <algorithm>

/* Public (Instance) -------------------------------------------------------- */

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

bool Listener::hasThisCookie(const std::pair<std::string, std::string>& pair)
	const
{
	return std::find(cookies_.begin(), cookies_.end(), pair) != cookies_.end();
}

std::pair<std::string, std::string> Listener::createBackgroundColorCookie()
{
	std::pair<std::string, std::string> cookie;
	cookie.first = "background-color";
	do
	{
		cookie.second = HexColorCode::generate();
	}
	while (this->hasThisCookie(cookie));
	cookies_.push_back(cookie);
	return cookie;
}
