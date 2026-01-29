#include "Response.hpp"
#include "HexColorCode.hpp"
#include "Helper.hpp"

/* Public (Static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, CookieJar* jar, Client& c)
{
	if (!c.route_info)
	{
		RequestData req = c.getRequestData();
		if (req.error_code != 0) {
    		return ResponseData(req.error_code);
  		}
		c.route_info = &router.getRoute(req);
		if (c.route_info.error_code != 0)
		{
    	return (c.route_info.error_code == 400 || c.route_info.error_code == 404) ? ResponseData(c.route_info.error_code)
                                    : ResponseData(c.route_info.error_code, c.route_info.server.errors);
  		}
	}
	if (c.route_info->cgi.is_cgi)
	{
		// Do stuff
		return "";
	}
	else if (request.method == "GET" || request.method == "HEAD") {
    return Get::handle(data);
  }
  else if (request.method == "POST") {
    return Post::handle(data);
  }
  else if (request.method == "DELETE") {
    return Delete::handle(data);
  }
  return ResponseData(500);


	/*
		TODO:
		- We need to get RouteInfo in all cases.
		- If it's CGI, do not proceed with the response (but do stuff).
		- Otherwise, call a function that will turn RouteInfo into ResponseData, 
		and proceed with the response.
	*/

	ResponseData res = router.handle(req);
	bool is_head = req.method == "HEAD";
	bool should_close = c.shouldCloseConnection() || res.code == 400;
	std::vector<std::string> cookie_headers;
	if (jar)
		jar->removeExpiredCookies();
	CookieJar::checkRequestCookies(jar, c, cookie_headers);
	CookieJar::generateCookieIfMissing(jar, c, cookie_headers);
	HexColorCode::embedBackgroundColor(c.getBackgroundColor(), res.content);
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
		str += getHeaderLine("Date", Helper::getDateGMT(std::time(0)));
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
