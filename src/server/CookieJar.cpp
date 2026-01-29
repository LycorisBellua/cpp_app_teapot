#include "CookieJar.hpp"
#include "HexColorCode.hpp"
#include <algorithm>

/* Public (Instance) -------------------------------------------------------- */

CookieJar::CookieJar(const std::string& ip) : ip_(ip)
{
}

bool CookieJar::hasThisCookie(const std::string& key, const std::string& value)
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

const Cookie& CookieJar::createBackgroundColorCookie()
{
	std::string key = "background-color";
	std::string value;
	do
	{
		value = HexColorCode::generate();
	}
	while (this->hasThisCookie(key, value));
	cookies_.push_back(Cookie(key, value, "", "/", 300));
	return cookies_.back();
}

void CookieJar::removeExpiredCookies()
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

void CookieJar::checkRequestCookies(const CookieJar* jar, Client& c,
	std::vector<std::string>& cookie_headers)
{
	const std::vector< std::pair<std::string, std::string> >& cookies
		= c.getCookies();
	bool any_valid = false;
	for (size_t i = 0; i < cookies.size(); ++i)
	{
		if (jar && cookies[i].first == "background-color"
			&& jar->hasThisCookie(cookies[i].first, cookies[i].second))
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

void CookieJar::generateCookieIfMissing(CookieJar* jar, Client& c,
	std::vector<std::string>& cookie_headers)
{
	if (!c.getBackgroundColor().empty())
		return;
	const Cookie& new_cookie = jar->createBackgroundColorCookie();
	cookie_headers.push_back(new_cookie.getSetCookieValue());
	c.setBackgroundColor(new_cookie.getValue());
}
