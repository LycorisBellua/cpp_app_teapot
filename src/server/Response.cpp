#include "Response.hpp"
#include "Cookie.hpp"
#include "Helper.hpp"
#include <ctime>

/* Public (Static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, Listener* listener,
	Client& c)
{
	RequestData req = c.getRequestData();
	ResponseData res = router.handle(req);
	bool is_head = req.method == "HEAD";
	bool should_close = c.shouldCloseConnection() || res.code == 400;
	std::vector<std::string> cookie_headers;
	Cookie::checkRequestCookies(listener, c, cookie_headers);
	Cookie::generateCookieIfMissing(listener, c, cookie_headers);
	Cookie::embedBackgroundColor(c.getBackgroundColor(), res.content);
	return Response::serialize(res, is_head, should_close, cookie_headers);
}

/* Private (Static) --------------------------------------------------------- */

std::string Response::serialize(const ResponseData& res, bool is_head,
	bool should_close, const std::vector<std::string>& cookie_headers)
{
	std::string str;
	str += Response::getStartLine(res.code, res.code_msg);
	if (res.code == 100)
		str += Response::getCRLF();
	else
	{
		str += getHeaderLine("Date", getCurrentDateGMT());
		str += getHeaderLine("Content-Length",
			Helper::nbrToString(res.content.length()));
		if (!res.content.empty())
			str += getHeaderLine("Content-Type", res.content_type);
		std::vector<std::string>::const_iterator itv;
		for (itv = cookie_headers.begin(); itv != cookie_headers.end(); ++itv)
			str += getHeaderLine("Set-Cookie", *itv);
		std::set< std::pair<std::string, std::string> >::iterator its;
		for (its = res.headers.begin(); its != res.headers.end(); ++its)
			str += getHeaderLine(its->first, its->second);
		if (should_close)
			str += getHeaderLine("Connection", "close");
		str += Response::getCRLF();
		if (!is_head)
			str += res.content;
	}
	return str;
}

std::string Response::getStartLine(int status, const std::string& msg)
{
	std::string str = "HTTP/1.1";
	str += " ";
	str += Helper::nbrToString(status);
	str += " ";
	str += msg;
	str += getCRLF();
	return str;
}

std::string Response::getHeaderLine(const std::string& key,
	const std::string& value)
{
	if (key.empty() || value.empty())
		return "";
	return key + ": " + value + getCRLF();
}

std::string Response::getCRLF()
{
	return "\r\n";
}

std::string Response::getCurrentDateGMT()
{
    std::time_t now = std::time(0);
    std::tm *gmt = std::gmtime(&now);
    if (!gmt)
		return "";
	char buffer[64] = {0};
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return buffer;
}
