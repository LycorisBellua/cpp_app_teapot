#include "Response.hpp"
#include "Get.hpp"
#include "Delete.hpp"
#include "Post.hpp"
#include "Helper.hpp"
#include <ctime>

/* Public (static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, const Client& c)
{
	//TODO: 100 Continue
	// if (c.getStatus() != 100)
	const RouteRequest& req = c.getRouteRequestData();
	const RouteResponse& res = router.getRoute(req);
	if (res.error_code)
	{
		// TODO: I already have the error page
	}
	else if (req.method == "GET" || req.method == "HEAD")
		return Response::serialize(c, req, Get::handle(res));
	else if (req.method == "DELETE")
		return Response::serialize(c, req, Delete::handle(res));
	else if (req.method == "POST")
		return Response::serialize(c, req, Post::handle(res));
	return "";
}

/* Private (static) --------------------------------------------------------- */

std::string Response::serialize(const Client& c, const RouteRequest& req,
	const HttpResponse& res)
{
	std::string str;
	str += Response::getStartLine(res.code, res.code_msg);
	if (req.error_code == 100)
		str += Response::getCRLF();
	else
	{
		str += Response::getDateLine();
		str += Response::getContentLengthLine(res.content.length());
		if (!res.content.empty())
			str += Response::getContentTypeLine(res.content_type);
		if (c.shouldCloseConnection())
			str += Response::getConnectionCloseLine();
		str += Response::getCRLF();
		if (req.method != "HEAD")
			str += res.content;
	}
	return str;
}

std::string Response::getCRLF()
{
	return "\r\n";
}

std::string Response::getStartLine(int status, const std::string& msg)
{
	std::string str = getVersion();
	str += " ";
	str += Helper::nbrToString(status);
	str += " ";
	str += msg;
	str += getCRLF();
	return str;
}

std::string Response::getVersion()
{
	return "HTTP/1.1";
}

std::string Response::getDateLine()
{
	return "Date: " + getCurrentDateGMT() + getCRLF();
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

std::string Response::getContentLengthLine(size_t length)
{
	return "Content-Length: " + Helper::nbrToString(length) + getCRLF();
}

std::string Response::getContentTypeLine(const std::string& type)
{
	return "Content-Type: " + type + getCRLF();
}

std::string Response::getConnectionCloseLine()
{
	return "Connection: close" + getCRLF();
}
