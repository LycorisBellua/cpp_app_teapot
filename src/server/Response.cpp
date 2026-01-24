#include "Response.hpp"
#include "Helper.hpp"
#include <ctime>

/* Public (Static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, const Client& c)
{
	/*
		TODO: The CGI can return HTTP headers to be sent. There's now a data 
		structure in the response structure containing all the properly 
		formatted key/value pairs.
		TODO: The CGI needs to be partly handled in the event loop.
	*/
	/*
		TODO
		- Client functions we have:
			std::vector< std::pair<std::string, std::string> > getCookies() const;
			std::string getHexBackgroundColor() const;
			bool setHexBackgroundColor(const std::string& str);
		- BackgroundColorCookie functions we have:
			static bool isValidName(const std::string& str);
			static bool isValidValue(const std::string& str);

		- Select the listener.
		- For each cookie, ask whether the listener has a copy of the cookie.
			- If not, add the cookie to a list marked for "deletion" (response 
			headers).
				`Set-Cookie: weirdCookie=value; Max-Age=0; Path=/`
			- If so, then call `c.setBackgroundColor(value)`.
		- If there was no saved value, it means either no cookie was sent or 
		none was valid.
			- Generate a hex code that's not already in use (in uppercase).
			- Store this cookie into the listener's cookie list.
			- Be aware that you'll have to use the `Set-Cookie` response header.
			- Call `c.setBackgroundColor(value)`.
	*/
	const RequestData& req = c.getRequestData();
	const ResponseData& res = router.handle(req);
	bool is_head = req.method == "HEAD";
	bool should_close = c.shouldCloseConnection() || res.code == 400;
	return Response::serialize(res, is_head, should_close);
}

/* Private (Static) --------------------------------------------------------- */

std::string Response::serialize(const ResponseData& res, bool is_head,
	bool should_close)
{
	std::string str;
	str += Response::getStartLine(res.code, res.code_msg);
	if (res.code == 100)
		str += Response::getCRLF();
	else
	{
		str += Response::getDateLine();
		str += Response::getContentLengthLine(res.content.length());
		if (!res.content.empty() && !res.content_type.empty())
			str += Response::getContentTypeLine(res.content_type);
		//TODO: Add all elements of `res.headers` if not empty
		if (should_close)
			str += Response::getConnectionCloseLine();
		str += Response::getCRLF();
		if (!is_head)
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
