#include "Response.hpp"
#include "Get.hpp"
#include "Delete.hpp"
#include "Post.hpp"
#include "Helper.hpp"
#include <ctime>

/* Public (Static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, const Client& c)
{
	const RequestData& req = c.getRequestData();
	Response::Adapter adapter;
	adapter.should_close = c.shouldCloseConnection();
	adapter.is_head = req.method == "HEAD";
	if (req.error_code == 100)
	{
		adapter.status = 100;
		adapter.status_msg = "Continue";
	}
        // TODO: change this to receive ResponseData
        (void)router;
	/*else
	{
		const RouteInfo& res = router.getRoute(req);
		if (res.error_code)
			adapter.setFromRouteResponse(res);
		else if (req.method == "GET" || req.method == "HEAD")
			adapter.setFromResponseData(Get::handle(res));
		else if (req.method == "POST")
			adapter.setFromResponseData(Post::handle(res));
		else if (req.method == "DELETE")
			adapter.setFromResponseData(Delete::handle(res));
		else
			return "";
	}*/
	return Response::serialize(adapter);
}

/* Private (Static) --------------------------------------------------------- */

std::string Response::serialize(const Response::Adapter& res)
{
	std::string str;
	str += Response::getStartLine(res.status, res.status_msg);
	if (res.status == 100)
		str += Response::getCRLF();
	else
	{
		str += Response::getDateLine();
		str += Response::getContentLengthLine(res.body.length());
		if (!res.body.empty() && !res.type.empty())
			str += Response::getContentTypeLine(res.type);
		if (res.should_close)
			str += Response::getConnectionCloseLine();
		str += Response::getCRLF();
		if (!res.is_head)
			str += res.body;
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

/* Private Nested Class ----------------------------------------------------- */

Response::Adapter::Adapter()
	: status(0), status_msg(""), body(""), type(""), should_close(false),
	is_head(false)
{
}

void Response::Adapter::setFromRouteResponse(const RouteInfo& res)
{
	this->status = res.error_code;
	this->status_msg = res.error_msg.empty();
	this->body = res.error_body;
	this->type = res.mime_type;
}

void Response::Adapter::setFromResponseData(const ResponseData& res)
{
	this->status = res.code;
	this->status_msg = res.code_msg;
	this->body = res.content;
	this->type = res.content_type;
}
