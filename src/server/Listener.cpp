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

bool Listener::hasThisCookie(const std::string& key, const std::string& value)
	const
{
	std::vector<Cookie>::const_iterator it;
	std::vector<Cookie>::const_iterator ite = cookies_.end();
	for (it = cookies_.begin(); it != ite; ++it)
	{
		if (key == it->getKey() && value == it->getValue())
			return true;
	}
	return false;
}

Cookie Listener::createBackgroundColorCookie()
{
	std::string key = "background-color";
	std::string value;
	do
	{
		value = HexColorCode::generate();
	}
	while (this->hasThisCookie(key, value));
	Cookie cookie(key, value, "", "/", 300);
	cookies_.push_back(cookie);
	return cookie;
}

void Listener::removeExpiredCookies()
{
	std::vector<Cookie>::iterator it;
	std::vector<Cookie>::iterator ite = cookies_.end();
	for (it = cookies_.begin(); it != ite;)
	{
		if (it->hasExpired())
			it = cookies_.erase(it);
		else
			++it;
	}
}

/* Public (Static) ---------------------------------------------------------- */

void Listener::checkRequestCookies(const Listener* listener, Client& c,
	std::vector<std::string>& cookie_headers)
{
	const std::vector< std::pair<std::string, std::string> >& cookies
		= c.getCookies();
	bool any_valid = false;
	for (size_t i = 0; i < cookies.size(); ++i)
	{
		if (listener && cookies[i].first == "background-color"
			&& listener->hasThisCookie(cookies[i].first, cookies[i].second))
		{
			c.setBackgroundColor(cookies[i].second);
			any_valid = true;
		}
		else
			cookie_headers.push_back(cookies[i].first + "=; Max-Age=0; Path=/");
	}
	if (!any_valid)
		c.setBackgroundColor("");
}

void Listener::generateCookieIfMissing(Listener* listener, Client& c,
	std::vector<std::string>& cookie_headers)
{
	if (!c.getBackgroundColor().empty())
		return;
	Cookie new_cookie = listener->createBackgroundColorCookie();
	cookie_headers.push_back(new_cookie.getSetCookieValue());
	c.setBackgroundColor(new_cookie.getValue());
}
