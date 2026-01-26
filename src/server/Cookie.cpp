#include "Cookie.hpp"
#include "Helper.hpp"

/* Public (Static) ---------------------------------------------------------- */

void Cookie::checkRequestCookies(const Listener* listener, Client& c,
	std::vector<std::string>& cookie_headers)
{
	const std::vector< std::pair<std::string, std::string> >& cookies
		= c.getCookies();
	bool any_valid = false;
	for (size_t i = 0; i < cookies.size(); ++i)
	{
		if (listener && cookies[i].first == "background-color"
			&& listener->hasThisCookie(cookies[i]))
		{
			c.setBackgroundColor(cookies[i].second);
			any_valid = true;
		}
		else
			cookie_headers.push_back(cookies[i].first + "=" + cookies[i].second
				+ "; Max-Age=0; Path=/");
	}
	if (!any_valid)
		c.setBackgroundColor("");
}

/* Generated cookie is kept by a client for only 300 seconds (= 5 minutes) */
void Cookie::generateCookieIfMissing(Listener* listener, Client& c,
	std::vector<std::string>& cookie_headers)
{
	if (!c.getBackgroundColor().empty())
		return;
	std::pair<std::string, std::string> new_cookie
		= listener->createBackgroundColorCookie();
	cookie_headers.push_back(new_cookie.first + "=" + new_cookie.second
		+ "; Max-Age=300; Path=/");
	c.setBackgroundColor(new_cookie.second);
}

/*
	END RESULT:

	`<body>`
		-> `<body style="background-color: #RRGGBB;">`

	`<body style="lorem: ipsum">`
		-> `<body style="background-color: #RRGGBB;lorem: ipsum">`

	`<body attr="">`
		-> `<body style="background-color: #RRGGBB;" attr="">`
*/
void Cookie::embedBackgroundColor(const std::string& color, std::string& html)
{
	size_t body_tag = findBodyTag(html);
	if (body_tag == std::string::npos)
		return;
	size_t opening_quote = insertStyleAttributeIfMissing(html, body_tag);
	html.insert(opening_quote + 1, "background-color: " + color + ";");
}

/* Private (Static) --------------------------------------------------------- */

size_t Cookie::findBodyTag(const std::string& html)
{
	if (!Helper::insensitiveCmp("<!DOCTYPE html>", html.substr(0, 15)))
		return std::string::npos;
	return html.find("<body");
}

size_t Cookie::insertStyleAttributeIfMissing(std::string& html, size_t body_tag)
{
	size_t attribute;
	if (html[body_tag + 5] == '>'
		|| (attribute = html.find("style=", body_tag + 5)) == std::string::npos)
	{
		html.insert(body_tag + 5, " style=\"\"");
		return body_tag + 12;
	}
	return attribute + 6;
}
