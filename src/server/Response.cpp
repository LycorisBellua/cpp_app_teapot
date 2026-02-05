#include "Response.hpp"
#include "HexColorCode.hpp"
#include "Helper.hpp"
#include "Cgi.hpp"
#include "Get.hpp"
#include "Post.hpp"
#include "Delete.hpp"

namespace
{
	void setRouteInfoAndResponseData(const Router& router, Client& c);
	std::string serialize(const ResponseData& res, bool is_head,
		bool should_close, const std::vector<std::string>& cookie_headers);
	std::string getStartLine(int status, const std::string& msg);
	std::string getHeaderLine(const std::string& key, const std::string& value);
	std::string getCRLF();
}

namespace Response
{
	std::string compose(const Router& router, CookieJar* jar, Client& c)
	{
		setRouteInfoAndResponseData(router, c);
		if (!c.response_data)
			return "";
		bool is_head = c.getMethod() == "HEAD";
		bool should_close = c.shouldCloseConnection()
			|| c.response_data->code == 400;
		std::vector<std::string> cookie_headers;
		if (jar)
			jar->removeExpiredCookies();
		CookieJar::checkRequestCookies(jar, c, cookie_headers);
		CookieJar::generateCookieIfMissing(jar, c, cookie_headers);
		HexColorCode::embedBackgroundColor(c.getBackgroundColor(),
			c.response_data->content);
		return serialize(*c.response_data, is_head, should_close,
			cookie_headers);
	}
}

namespace
{
	void setRouteInfoAndResponseData(const Router& router, Client& c)
	{
		if (c.isCgiRunning() || c.response_data)
			return;
		if (!c.route_info)
		{
			RequestData req = c.getRequestData();
			if (req.error_code)
			{
				c.response_data = new ResponseData(req.error_code);
				return;
			}
			c.route_info = new RouteInfo(router.getRoute(req));
			if (c.route_info->error_code)
			{
				if (c.route_info->error_code == 301
					|| c.route_info->error_code == 302)
				{
					c.response_data = new ResponseData;
					c.response_data->code = c.route_info->error_code;
					c.response_data->headers.insert(
						std::make_pair<std::string, std::string>("Location",
							c.route_info->location.redirect.second));
				}
				else if (c.route_info->error_code == 400
					|| c.route_info->error_code == 404)
					c.response_data =
						new ResponseData(c.route_info->error_code);
				else
					c.response_data = new ResponseData(c.route_info->error_code,
						c.route_info->server.errors);
				return;
			}
		}
		if (c.isCgiRunning())
		{
			c.response_data = Cgi::handle(*c.route_info);
			if (c.response_data)
				c.route_info->cgi.is_cgi = false;
		}
		else if (c.route_info->request.method == "GET"
			|| c.route_info->request.method == "HEAD")
			c.response_data = new ResponseData(Get::handle(*c.route_info));
		else if (c.route_info->request.method == "POST")
			c.response_data = new ResponseData(Post::handle(*c.route_info));
		else if (c.route_info->request.method == "DELETE")
			c.response_data = new ResponseData(Delete::handle(*c.route_info));
		else
			c.response_data = new ResponseData(500);
	}

	std::string serialize(const ResponseData& res, bool is_head,
		bool should_close, const std::vector<std::string>& cookie_headers)
	{
		std::string str;
		str += getStartLine(res.code, res.code_msg);
		if (res.code == 100)
			str += getCRLF();
		else
		{
			str += getHeaderLine("Date", Helper::getDateGMT(std::time(0)));
			str += getHeaderLine("Content-Length",
				Helper::nbrToString(res.content.length()));
			if (!res.content.empty())
				str += getHeaderLine("Content-Type", res.content_type);
			std::vector<std::string>::const_iterator itv;
			for (itv = cookie_headers.begin(); itv != cookie_headers.end();
				++itv)
				str += getHeaderLine("Set-Cookie", *itv);
			std::set< std::pair<std::string, std::string> >::iterator its;
			for (its = res.headers.begin(); its != res.headers.end(); ++its)
				str += getHeaderLine(its->first, its->second);
			if (should_close)
				str += getHeaderLine("Connection", "close");
			str += getCRLF();
			if (!is_head)
				str += res.content;
		}
		return str;
	}

	std::string getStartLine(int status, const std::string& msg)
	{
		std::string str = "HTTP/1.1";
		str += " ";
		str += Helper::nbrToString(status);
		str += " ";
		str += msg;
		str += getCRLF();
		return str;
	}

	std::string getHeaderLine(const std::string& key,
		const std::string& value)
	{
		if (key.empty() || value.empty())
			return "";
		return key + ": " + value + getCRLF();
	}

	std::string getCRLF()
	{
		return "\r\n";
	}
}
